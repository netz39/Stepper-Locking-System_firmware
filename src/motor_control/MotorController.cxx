#include "MotorController.hpp"
#include "sync.hpp"

#include <cstdio>
#include <cstring>

constexpr auto BufferSize = 128;
char buffer[BufferSize];

void MotorController::taskMain()
{
    sync::waitForAll(sync::ConfigurationLoaded);
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        snprintf(buffer, BufferSize, "motor position: %ld\n", stepperMotor.getPosition());
        HAL_UART_Transmit(DebugUartPeripherie, reinterpret_cast<uint8_t *>(buffer), strlen(buffer),
                          1000);

        // ToDo:
        // check current movement - stepControl.getCurrentSpeed()
        // compare with values from hall encoder

        if (isCalibrating && !stepControl.isRunning())
        {
            disableCalibrationMode();

            // calibration was not successful
            if (callback)
                callback(false);
        }

        checkMotorTemperature();

        vTaskDelayUntil(&lastWakeTime, toOsTicks(50.0_Hz));
    }

    // deal with cases like loosing steps, obstacle while moving
}

//--------------------------------------------------------------------------------------------------
void MotorController::onSettingsUpdate()
{
    maximumMotorCurrentInPercentage =
        settingsContainer.getValue<firmwareSettings::MotorMaxCurrent>();
    maximumMotorSpeed = settingsContainer.getValue<firmwareSettings::MotorMaxSpeed>();
    maximumMotorAcc = settingsContainer.getValue<firmwareSettings::MotorMaxAcc>();
    calibrationSpeed = settingsContainer.getValue<firmwareSettings::CalibrationSpeed>();
    calibrationAcc = settingsContainer.getValue<firmwareSettings::CalibrationAcc>();

    overheatedCounter = settingsContainer.getValue<firmwareSettings::MotorOverheatCounter>();
    warningTempCounter = settingsContainer.getValue<firmwareSettings::MotorWarningTempCounter>();

    // set to new parameters
    isInCalibrationMode ? enableCalibrationMode() : disableCalibrationMode();
}

//--------------------------------------------------------------------------------------------------
void MotorController::finishedCallback()
{
    isOpening = false;
    isClosing = false;

    if (!ignoreFinishedEvent)
    {
        disableMotorTorque();

        if (callback)
            callback(true);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::moveAbsolute(int32_t position)
{
    if (!isOverheated && !isMotorFreezed)
    {
        enableMotorTorque();

        stepperMotor.setTargetAbs(position);
        stepControl.moveAsync(stepperMotor);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::moveRelative(int32_t microSteps)
{
    if (!isOverheated && !isMotorFreezed)
    {
        enableMotorTorque();

        stepperMotor.setTargetRel(microSteps);
        stepControl.moveAsync(stepperMotor);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::openDoor()
{
    isOpening = true;
    moveAbsolute(isDirectionInverted ? 1 : -1 * NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::closeDoor()
{
    isClosing = true;
    moveAbsolute(0);
}

//--------------------------------------------------------------------------------------------------

void MotorController::doCalibration(bool forceInverted)
{
    bool invert = isDirectionInverted != forceInverted;

    enableCalibrationMode();
    moveRelative((invert ? -1.0f : 1.0f) * NumberOfMicrostepsForOperation * 1.25f);

    isCalibrating = true;
}

//--------------------------------------------------------------------------------------------------
void MotorController::abortCalibration()
{
    stopMovement();
    disableCalibrationMode();
    isCalibrating = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::calibrationIsDone()
{
    abortCalibration();
    stepperMotor.setPosition(0);

    // ToDo: get hall encoder value and save it
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

    stepperMotor.setMaxSpeed(calibrationSpeed);
    stepperMotor.setAcceleration(calibrationAcc);
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

//--------------------------------------------------------------------------------------------------
void MotorController::revertOpening()
{
    ignoreFinishedEvent = true;
    stopMovement();
    closeDoor();
    ignoreFinishedEvent = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::revertClosing()
{
    ignoreFinishedEvent = true;
    stopMovement();
    openDoor();
    ignoreFinishedEvent = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::freezeMotor()
{
    stopMovementImmediately();
    isMotorFreezed = true;
}

//--------------------------------------------------------------------------------------------------
void MotorController::unfreezeMotor()
{
    isMotorFreezed = false;
}

//--------------------------------------------------------------------------------------------------
uint8_t MotorController::getProgress()
{
    if (!isOpening && !isClosing)
        return 100;

    const auto Target =
        isOpening ? (isDirectionInverted ? 1 : -1 * NumberOfMicrostepsForOperation) : 0;

    const auto Diff = std::abs(Target - stepperMotor.getPosition());
    const uint8_t Percentage =
        ((NumberOfMicrostepsForOperation - Diff) * 100) / NumberOfMicrostepsForOperation;
    return std::clamp<uint8_t>(Percentage, 0, 100);
}