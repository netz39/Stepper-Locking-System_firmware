#include "MotorController.hpp"

void MotorController::taskMain()
{
    if (stepControl.isRunning())
    {
        // stepControl.getCurrentSpeed()

        // check current movement
        // compare with values from hall encoder
    }

    // check motor temperature
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

    // deal with cases like loosing steps, obstacle while moving
    // do calibration when needed
}

//--------------------------------------------------------------------------------------------------
void MotorController::onSettingsUpdate()
{
    maximumMotorCurrentInPercentage =
        settingsContainer.getValue<firmwareSettings::MotorMaxCurrent, uint8_t>();

    maximumMotorSpeed = settingsContainer.getValue<firmwareSettings::MotorMaxSpeed>();
    maximumMotorAcc = settingsContainer.getValue<firmwareSettings::MotorMaxAcc>();

    overheatedCounter = settingsContainer.getValue<firmwareSettings::MotorOverheatCounter>();
    warningTempCounter = settingsContainer.getValue<firmwareSettings::MotorWarningTempCounter>();
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
void MotorController::homeLockSwitch()
{
    enableCalibrationMode();
    moveRelative(isDirectionInverted ? -1 : 1 * NumberOfMicrostepsForOperation);
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
    stopMovement();
    // TODO: restore settings
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