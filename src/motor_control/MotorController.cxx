#include "MotorController.hpp"

void MotorController::taskMain()
{
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        // ToDo:
        // check current movement - stepControl.getCurrentSpeed()
        // compare with values from hall encoder

        if (isCalibrating() && !stepControl.isRunning())
        {
            // calibration was not successful, retry it
            doCalibration();
        }

        checkMotorTemperature();

        vTaskDelayUntil(&lastWakeTime, toOsTicks(100.0_Hz));
    }

    // deal with cases like loosing steps, obstacle while moving
}

//--------------------------------------------------------------------------------------------------
void MotorController::onSettingsUpdate()
{

    maximumMotorCurrentInPercentage = settingsContainer.getDefaultValue(
        settingsContainer.getIndex<firmwareSettings::MotorMaxCurrent>());
    maximumMotorSpeed = settingsContainer.getDefaultValue(
        settingsContainer.getIndex<firmwareSettings::MotorMaxSpeed>());
    maximumMotorAcc = settingsContainer.getDefaultValue(
        settingsContainer.getIndex<firmwareSettings::MotorMaxAcc>());

    overheatedCounter = settingsContainer.getDefaultValue(
        settingsContainer.getIndex<firmwareSettings::MotorOverheatCounter>());
    warningTempCounter = settingsContainer.getDefaultValue(
        settingsContainer.getIndex<firmwareSettings::MotorWarningTempCounter>());

    if (!isInCalibrationMode)
        disableCalibrationMode(); // set parameters in normal mode
}

//--------------------------------------------------------------------------------------------------
void MotorController::finishedCallback()
{
    disableMotorTorque();
}

//--------------------------------------------------------------------------------------------------
void MotorController::moveRelative(int32_t microSteps)
{
    if (!isOverheated)
    {
        enableMotorTorque();

        stepperMotor.setTargetRel(microSteps);
        stepControl.moveAsync(stepperMotor);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::openDoor()
{
    moveRelative(isDirectionInverted ? 1 : -1 * NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::closeDoor()
{
    moveRelative(isDirectionInverted ? -1 : 1 * NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------

void MotorController::doCalibration()
{
    enableCalibrationMode();
    moveRelative(isDirectionInverted ? -1 : 1 * NumberOfMicrostepsForOperation * 1.25f);
}

//--------------------------------------------------------------------------------------------------
bool MotorController::isCalibrating() const
{
    return isInCalibrationMode;
}

//--------------------------------------------------------------------------------------------------
void MotorController::calibrationIsDone()
{
    stopMovement();
    disableCalibrationMode();
}

//--------------------------------------------------------------------------------------------------
void MotorController::stopMovement()
{
    stepControl.stop();
}

//--------------------------------------------------------------------------------------------------
void MotorController::stopMovementImmediately()
{
    stepControl.emergencyStop();
    disableMotorTorque();
}

//--------------------------------------------------------------------------------------------------
void MotorController::enableCalibrationMode()
{
    isInCalibrationMode = true;
    stopMovement();
    stepperMotor.setMaxSpeed(CalibrationSpeed);
    stepperMotor.setAcceleration(CalibrationAcc);
    setMotorMaxCurrentPercentage(100);
}

//--------------------------------------------------------------------------------------------------
void MotorController::disableCalibrationMode()
{
    isInCalibrationMode = false;

    setMotorMaxCurrentPercentage(maximumMotorCurrentInPercentage);
    stepperMotor.setMaxSpeed(maximumMotorSpeed);
    stepperMotor.setAcceleration(maximumMotorAcc);
}

//--------------------------------------------------------------------------------------------------
void MotorController::setMotorMaxCurrentPercentage(uint8_t percentage)
{
    if (percentage > 100)
        percentage = 100;

    TMC2209::IHoldRun iHoldRun{};
    constexpr auto TotalCurrentSteps = 31; // 4 Bit values
    const uint32_t NewRegisterValue = std::round(percentage / 100.0f * TotalCurrentSteps);

    iHoldRun.iholdrun.ihold = NewRegisterValue;
    iHoldRun.iholdrun.irun = NewRegisterValue;
    if (!tmc2209.setIHoldRun(iHoldRun))
    {
        // TODO: report error
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::enableMotorTorque()
{
    stepperEnable.write(false);
}

//--------------------------------------------------------------------------------------------------
void MotorController::disableMotorTorque()
{
    stepperEnable.write(true);
}

//--------------------------------------------------------------------------------------------------
void MotorController::checkMotorTemperature()
{
    if (motorTemperature >= CriticalMotorTemp)
    {
        if (!isOverheated)
            overheatedCounter++;

        isOverheated = true;
        hasWarningTemp = false;
    }
    else if (motorTemperature >= WarningMotorTemp)
    {
        if (!hasWarningTemp && !isOverheated)
            warningTempCounter++;

        isOverheated = false;
        hasWarningTemp = true;
    }
    else
    {
        isOverheated = false;
        hasWarningTemp = false;
    }
}
