#include "MotorController.hpp"
#include "sync.hpp"

#include <cstdio>
#include <cstring>

constexpr auto BufferSize = 256;
char buffer[BufferSize];

void MotorController::taskMain()
{
    sync::waitForAll(sync::ConfigurationLoaded | sync::StateMachineStarted);
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        if (isCalibrating)
        {
            // check if there is no calibration movement anymore
            if (!stepControl.isRunning())
            {
                // calibration was not successful
                isCalibrating = false;
                disableCalibrationMode();
                signalFailure(FailureType::CalibrationFailed);
            }

            // check for step losses while calibrating
            /*
            else if (hallEncoder.isEncoderOkay() &&
                     (std::abs(stepperMotor.getPosition() - hallEncoder.getPosition()) >
                      MicrostepLossThreshold))
            {
                // step losses occured, count it
                if (stepLossEventCounter++ >= StepLossEventAtCalibrationCounterThreshold)
                {
                    isCalibrating = false;
                    signalFailure(FailureType::CalibrationFailed);
                    stepLossEventCounter = 0;
                }
            }
            */
        }
        else if (isCalibrated && hallEncoder.isEncoderOkay())
        {
            // check for step losses while normal movement
            if (std::abs(stepperMotor.getPosition() - hallEncoder.getPosition()) >
                MicrostepLossThreshold)
            {
                // step losses occured, update steppers internal position to hall encoder
                // movement will be corrected by TeensyStep
                stepperMotor.setPosition(hallEncoder.getPosition());

                if (stepControl.isRunning())
                {
                    if (stepLossEventCounter++ >= StepLossEventCounterThreshold)
                    {
                        signalFailure(FailureType::ExcessiveStepLosses);
                        stepLossEventCounter = 0;
                    }
                }
                else
                {
                    // motor is moving externally
                    signalFailure(FailureType::MotorMovedExternally);
                }
            }
        }

        if (isOpening || isClosing)
        {
            snprintf(buffer, BufferSize, "%ld, %ld, %d\n", stepperMotor.getPosition(),
                     hallEncoder.getPosition(), hallEncoder.getRawPosition());
            HAL_UART_Transmit(DebugUartPeripherie, reinterpret_cast<uint8_t *>(buffer),
                              strlen(buffer), 1000);
        }

        checkMotorTemperature();

        vTaskDelayUntil(&lastWakeTime, toOsTicks(100.0_Hz));
    }
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

    stepperMotor.setInverseRotation(
        settingsContainer.getValue<firmwareSettings::InvertRotationDirection>());

    // set to new parameters
    isInCalibrationMode ? enableCalibrationMode() : disableCalibrationMode();
}

//--------------------------------------------------------------------------------------------------
void MotorController::invokeFinishedCallback()
{
    stepLossEventCounter = 0;
    isOpening = false;
    isClosing = false;

    if (!ignoreFinishedEvent)
    {
        disableMotorTorque();
        signalSuccess();
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::signalSuccess()
{
    if (finishedCallback)
        finishedCallback(FailureType::None);
}

//--------------------------------------------------------------------------------------------------
void MotorController::signalFailure(FailureType failureType)
{
    if (finishedCallback)
        finishedCallback(failureType);
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
    moveAbsolute(NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::closeDoor()
{
    isClosing = true;
    moveAbsolute(0);
}

//--------------------------------------------------------------------------------------------------
void MotorController::manualOpen()
{
    unfreezeMotor();
    applyCalibrationMotorSettings();
    moveRelative(2 * NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::manualClose()
{
    unfreezeMotor();
    applyCalibrationMotorSettings();
    moveRelative(-2 * NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::disableManualMode()
{
    stopMovementImmediately();
    applyNormalMotorSettings();
}

//--------------------------------------------------------------------------------------------------
void MotorController::revokeCalibration()
{
    isCalibrated = false;
}

//--------------------------------------------------------------------------------------------------

void MotorController::doCalibration(bool forceInverted)
{
    enableCalibrationMode();
    const auto NeededSteps = static_cast<int32_t>((forceInverted ? 1.0f : -1.0f) *
                                                  NumberOfMicrostepsForOperation * 1.25f);
    moveRelative(NeededSteps);

    isCalibrated = false;
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

    vTaskDelay(10); // wait for new steady value from hall encoder
    stepperMotor.setPosition(0);
    hallEncoder.setPosition(0);
    isCalibrated = true;
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
void MotorController::applyCalibrationMotorSettings()
{
    setMotorMaxCurrentPercentage(100);
    stepperMotor.setMaxSpeed(calibrationSpeed);
    stepperMotor.setAcceleration(calibrationAcc);
}

//--------------------------------------------------------------------------------------------------
void MotorController::applyNormalMotorSettings()
{
    setMotorMaxCurrentPercentage(maximumMotorCurrentInPercentage);
    stepperMotor.setMaxSpeed(maximumMotorSpeed);
    stepperMotor.setAcceleration(maximumMotorAcc);
}

//--------------------------------------------------------------------------------------------------
void MotorController::enableCalibrationMode()
{
    isInCalibrationMode = true;
    applyCalibrationMotorSettings();
}

//--------------------------------------------------------------------------------------------------
void MotorController::disableCalibrationMode()
{
    isInCalibrationMode = false;
    applyNormalMotorSettings();
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
        return 0;

    const auto Target = isOpening ? NumberOfMicrostepsForOperation : 0;
    const auto Diff = std::abs(Target - stepperMotor.getPosition());
    const uint8_t Percentage =
        ((NumberOfMicrostepsForOperation - Diff) * 100) / NumberOfMicrostepsForOperation;
    return std::clamp<uint8_t>(Percentage, 0, 100);
}