#include "MotorController.hpp"
#include "sync.hpp"

#include <cstdio>
#include <cstring>

constexpr auto BufferSize = 256;
char buffer[BufferSize];

using util::wrappers::NotifyAction;

[[noreturn]] void MotorController::taskMain()
{
    sync::waitForAll(sync::ConfigurationLoaded | sync::StateMachineStarted);

    setSendDelayToMax();

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        vTaskDelayUntil(&lastWakeTime, toOsTicks(50.0_Hz));

        if (shouldSendSignalSuccess)
        {
            // signal flag from ISR context arrived, send it without ISR context
            shouldSendSignalSuccess = false;
            signalSuccess();
        }

        if (!checkStepperDriver())
            continue;

        checkHallEncoder();

        if (isCalibrating)
            checkCalibration();

        else if (isCalibrated)
            checkMovement();

        /*
        // print data over UART while moving
        if (isOpening || isClosing)
        {
            snprintf(buffer, BufferSize, "%ld, %ld, %d\n", stepperMotor.getPosition(),
                     hallEncoder.getPosition(), hallEncoder.getRawPosition());

            static constexpr auto DebugUartPeripherie = &huart1;
            HAL_UART_Transmit(DebugUartPeripherie, reinterpret_cast<uint8_t *>(buffer),
                              strlen(buffer), 1000);
        }
        */

        checkMotorTemperature();
    }
}

//--------------------------------------------------------------------------------------------------
bool MotorController::checkStepperDriver()
{
    // ping TMC with a request for motor load
    [[maybe_unused]] const auto MotorLoad = tmc2209.getSGResult().value_or(TMC2209::SGResult{});

    if (tmc2209.isCommFailure())
    {
        if (!hadTmcFailure)
            settingsContainer.addToValue<firmwareSettings::TmcFailureCounter>(1);

        if (uartFailureCounter++ >= UartFailueCounterThreshold)
        {
            hadTmcFailure = true;
            signalFailure(FailureType::StepperDriverNoAnswer);
            return false;
        }
    }
    else if (hadTmcFailure)
    {
        // TMC is reachable again, send sucess signal
        hadTmcFailure = false;
        uartFailureCounter = 0;
        signalSuccess();
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
void MotorController::checkHallEncoder()
{
    if (!hallEncoder.isOkay())
    {
        if (!hadHallEncoderFailure)
            settingsContainer.addToValue<firmwareSettings::HallFailureCounter>(1);

        // send warning, but door can open/close normally
        hadHallEncoderFailure = true;

        if (isCalibrating)
            abortCalibration();

        if (isCalibrated)
            signalFailure(FailureType::HallEncoderNoAnswer);
        else
            signalFailure(FailureType::CalibrationFailed);
    }
    else if (hadHallEncoderFailure)
    {
        // send signal for recalibrating
        hadHallEncoderFailure = false;
        signalFailure(FailureType::HallEncoderReconnected);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::checkCalibration()
{
    // check if there is no calibration movement anymore
    if (!stepControl.isRunning())
    {
        // calibration was not successful
        abortCalibration();
        signalFailure(FailureType::CalibrationFailed);
    }

    // check for step losses while calibrating
    if (checkForStepLosses())
    {
        // step losses occured, count it
        if (eventCounter++ >= StepLossEventAtCalibrationCounterThreshold)
        {
            settingsContainer.addToValue<firmwareSettings::CriticalStepLossCounter>(1);
            isCalibrating = false;
            stopMovementImmediately();
            signalFailure(FailureType::CalibrationFailed);
            eventCounter = 0;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::checkMovement()
{
    // check for step losses while normal movement
    if (checkForStepLosses())
    {
        // step losses occured, update steppers internal position to hall encoder
        // movement will be corrected by TeensyStep
        stepperMotor.setPosition(hallEncoder.getPosition());

        if (stepControl.isRunning())
        {
            if (eventCounter++ >= StepLossEventCounterThreshold)
            {

                settingsContainer.addToValue<firmwareSettings::CriticalStepLossCounter>(1);
                stopMovementImmediately();
                signalFailure(FailureType::ExcessiveStepLosses);
                eventCounter = 0;
            }
        }
        else
        {
            if (eventCounter++ >= ExternalMotorMoveEventCounterThreshold)
            {
                settingsContainer.addToValue<firmwareSettings::ExternalMotorMovementCounter>(1);
                // motor is moving externally
                signalFailure(FailureType::MotorMovedExternally);
                eventCounter = 0;
            }
        }
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

    stepperMotor.setInverseRotation(
        settingsContainer.getValue<firmwareSettings::InvertRotationDirection>());

    // set to new parameters
    isInCalibrationMode ? enableCalibrationMode() : disableCalibrationMode();
}

//--------------------------------------------------------------------------------------------------
void MotorController::callbackFromTeensyStepISR()
{
    eventCounter = 0;
    resetOpeningClosingState();

    if (!ignoreFinishedEvent)
    {
        disableMotorTorque();

        // do not call signalSuccess directly because this call is coming from interrupt context
        // set flag so motor controller task will send the success signal in normal context
        shouldSendSignalSuccess = true;
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::signalSuccess()
{
    if (notifyStateMaschine)
        notifyStateMaschine(FailureType::None);
}

//--------------------------------------------------------------------------------------------------
void MotorController::signalFailure(FailureType failureType)
{
    if (notifyStateMaschine)
        notifyStateMaschine(failureType);
}

//--------------------------------------------------------------------------------------------------
void MotorController::moveAbsolute(int32_t position, bool async)
{
    if (!isOverheated && !isMotorFreezed)
    {
        enableMotorTorque();

        stepperMotor.setTargetAbs(position);

        if (async)
            stepControl.moveAsync(stepperMotor);
        else
            stepControl.move(stepperMotor);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::moveRelative(int32_t microSteps, bool async)
{
    if (!isOverheated && !isMotorFreezed)
    {
        enableMotorTorque();

        stepperMotor.setTargetRel(microSteps);

        if (async)
            stepControl.moveAsync(stepperMotor);
        else
            stepControl.move(stepperMotor);
    }
}

//--------------------------------------------------------------------------------------------------
void MotorController::openDoor()
{
    setOpeningState();
    moveAbsolute(NumberOfMicrostepsForOperation);
}

//--------------------------------------------------------------------------------------------------
void MotorController::closeDoor()
{
    setClosingState();
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

    if (isCalibrating)
    {
        settingsContainer.addToValue<firmwareSettings::AbortCalibrationCounter>(1);
        abortCalibration();
    }
}

//--------------------------------------------------------------------------------------------------

void MotorController::doCalibration(bool forceInverted)
{
    forceInverted
        ? settingsContainer.addToValue<firmwareSettings::CalibrationInverseCommandCounter>(1)
        : settingsContainer.addToValue<firmwareSettings::CalibrationCommandCounter>(1);

    enableCalibrationMode();

    // only do it to get comparable values to detect step losses reliable
    stepperMotor.setPosition(0);
    hallEncoder.saveHomePosition();
    eventCounter = 0;

    const auto NeededSteps = static_cast<int32_t>((forceInverted ? 1.0f : -1.0f) *
                                                  NumberOfMicrostepsForOperation * 1.25f);
    moveRelative(NeededSteps);

    isCalibrated = false;
    isCalibrating = true;
}

//--------------------------------------------------------------------------------------------------
void MotorController::abortCalibration()
{
    isCalibrating = false;
    stopMovement();
    disableCalibrationMode();
}

//--------------------------------------------------------------------------------------------------
void MotorController::calibrationIsDone()
{
    settingsContainer.addToValue<firmwareSettings::CompleteCalibrationCounter>(1);

    // add offset to move the stepper a little bit further
    // this should ensure that the lock switch is triggered in every case to turn off the Nerdstern
    stepperMotor.setPosition(CalibrationOffsetInMicrosteps);
    moveAbsolute(0, false);

    abortCalibration();

    vTaskDelay(10); // wait for new steady value from hall encoder
    stepperMotor.setPosition(0);

    if (hallEncoder.saveHomePosition())
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
    if (adc.getMotorTemperature() >= CriticalMotorTemp)
    {
        if (!isOverheated)
            settingsContainer.addToValue<firmwareSettings::MotorOverheatCounter>(1);

        isOverheated = true;
        hasWarningTemp = false;
    }
    else if (adc.getMotorTemperature() >= WarningMotorTemp)
    {
        if (!hasWarningTemp && !isOverheated)
            settingsContainer.addToValue<firmwareSettings::MotorWarningTempCounter>(1.0f);

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
bool MotorController::checkForStepLosses()
{
    if (!hallEncoder.isOkay())
        return false;

    return (std::abs(stepperMotor.getPosition() - hallEncoder.getPosition()) >
            MicrostepLossThreshold);
}

//--------------------------------------------------------------------------------------------------
void MotorController::revertOpening()
{
    setClosingState();
    ignoreFinishedEvent = true;
    stopMovement();
    closeDoor();
    ignoreFinishedEvent = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::revertClosing()
{
    setOpeningState();
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
uint8_t MotorController::getProgress() const
{
    if (!isOpening && !isClosing)
        return 0;

    const auto Target = isOpening ? NumberOfMicrostepsForOperation : 0;
    const auto Diff = std::abs(Target - stepperMotor.getPosition());
    const uint8_t Percentage =
        ((NumberOfMicrostepsForOperation - Diff) * 100) / NumberOfMicrostepsForOperation;
    return std::clamp<uint8_t>(Percentage, 0, 100);
}

//--------------------------------------------------------------------------------------------------
void MotorController::setOpeningState()
{
    settingsContainer.addToValue<firmwareSettings::OpenCommandCounter>(1);
    isOpening = true;
    isClosing = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::setClosingState()
{
    settingsContainer.addToValue<firmwareSettings::CloseCommandCounter>(1);
    isOpening = false;
    isClosing = true;
}

//--------------------------------------------------------------------------------------------------
void MotorController::resetOpeningClosingState()
{
    if (isOpening)
        settingsContainer.addToValue<firmwareSettings::CompleteOpeningCounter>(1);

    else if (isClosing)
        settingsContainer.addToValue<firmwareSettings::CompleteClosingCounter>(1);

    isOpening = false;
    isClosing = false;
}

//--------------------------------------------------------------------------------------------------
void MotorController::notifyUartTxComplete()
{
    auto higherPriorityTaskWoken = pdFALSE;
    notifyFromISR(1, NotifyAction::SetBits, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//--------------------------------------------------------------------------------------------------
void MotorController::notifyUartRxComplete()
{
    auto higherPriorityTaskWoken = pdFALSE;
    notifyFromISR(1, NotifyAction::SetBits, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

//--------------------------------------------------------------------------------------------------
bool MotorController::setSendDelayToMax()
{
    TMC2209::SlaveConfig slaveConf{};
    slaveConf.slaveConfig = 15 << 8;
    return tmc2209.setSlaveConfig(slaveConf);
}