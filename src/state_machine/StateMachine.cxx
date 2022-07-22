#include "StateMachine.hpp"
#include "sync.hpp"
#include "main.h"

#include <climits>

using util::Button;
using util::wrappers::NotifyAction;

// todo too complex, simplify by putting each state into its own function
[[noreturn]] void StateMachine::taskMain()
{
    // wait some time to get steady switches states
    vTaskDelay(toOsTicks(550.0_ms));

    sync::waitForAll(sync::ConfigurationLoaded);
    sync::signal(sync::StateMachineStarted);

    while (true)
    {
        switch (currentState)
        {

        //------------------------------
        case State::Initializing:
        {
            motorController.unfreezeMotor();
            if (tactileSwitches.doorSwitch.isPressing())
            { // door wing is triggering doorSwitch

                // calibrate door
                currentState = State::Calibrating;
            }
            else
            {
                // door wing is open, so we assume door and lock bolt are correctly open
                // we will calibrate later at the moment, when the door should close
                currentState = State::Opened;
            }
        }
        break;

        //------------------------------
        case State::Opened:
        {
            if (!waitForCloseCommand())
                break;

            if (tactileSwitches.doorSwitch.isPressing())
            { // door wing is triggering doorSwitch

                if (isCalibrated)
                {
                    motorController.closeDoor();
                    currentState = State::Closing;
                }
                else
                    currentState = State::Calibrating;
            }
            else
            {
                // door wing not closed, keep close command in mind
                currentState = State::WantToClose;
            }
        }
        break;

        //------------------------------
        case State::Closed:
        {
            if (!waitForOpenCommand())
                break;

            motorController.openDoor();
            currentState = State::Opening;
        }
        break;

        //------------------------------
        case State::Opening:
        {
            if (!waitForFinishedEvent())
                break;

            currentState = State::Opened;
        }
        break;

        //------------------------------
        case State::Closing:
        {
            if (!waitForFinishedEvent())
                break;

            currentState = State::Closed;
            break;
        }

        //------------------------------
        case State::WantToClose:
        {
            if (!waitForDoorStateTriggered())
                break;

            if (isCalibrated)
            {
                motorController.closeDoor();
                currentState = State::Closing;
            }
            else
                currentState = State::Calibrating;
        }
        break;

        //------------------------------
        case State::RetryToClose:
        {
            if (!waitForFinishedEvent())
                break;

            vTaskDelay(toOsTicks(1.0_s));
            currentState = State::WantToClose;
        }
        break;

        //------------------------------
        case State::Calibrating:
        {
            if (!tactileSwitches.lockSwitch.isPressing())
            { // lock switch already triggered, so opens the door until the lock switch is released

                motorController.doCalibration(true);

                if (!waitForLockStateReleased())
                    break;

                motorController.abortCalibration();
                vTaskDelay(toOsTicks(50.0_ms));
            }

            motorController.doCalibration();

            if (!waitForLockStateTriggered())
                break;

            motorController.calibrationIsDone();
            isCalibrated = true;
            currentState = State::Closed;
        }
        break;

        //------------------------------
        default:
        case State::Warning:
            vTaskDelay(toOsTicks(50.0_ms));
            break;

        //------------------------------
        case State::FatalError:
            motorController.freezeMotor();
            vTaskDelay(toOsTicks(50.0_ms));
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForCommand(const uint32_t eventBit, const TickType_t xTicksToWait)
{
    while (true)
    {
        uint32_t notifiedValue{};
        if (notifyWait(ULONG_MAX, ULONG_MAX, &notifiedValue, xTicksToWait))
        {
            if ((notifiedValue & ErrorBit) != 0)
                return false;

            if ((notifiedValue & eventBit) != 0)
                return true;
        }
        else
            // timeout occurred
            return false;
    }
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForOpenCommand()
{
    return waitForCommand(OpenCommandBit, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForCloseCommand()
{
    return waitForCommand(CloseCommandBit, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForDoorStateTriggered()
{
    if (tactileSwitches.doorSwitch.isPressing())
        return true;

    return waitForCommand(DoorStateTriggerBit, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForLockStateTriggered()
{
    if (!tactileSwitches.lockSwitch.isPressing())
        return true;

    return waitForCommand(LockStateTriggerBit, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForLockStateReleased()
{
    if (tactileSwitches.lockSwitch.isPressing())
        return true;

    return waitForCommand(LockStateReleaseBit, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForFinishedEvent()
{
    return waitForCommand(FinishedEvent, portMAX_DELAY); //todo reasonable timeout instead of max_delay
}

//--------------------------------------------------------------------------------------------------
void StateMachine::openButtonCallback(const util::Button::Action action)
{
    switch (action)
    {
    case Button::Action::ShortPress:
        if (currentState == State::FatalError)
            currentState = State::Initializing;

        [[fallthrough]];

    case Button::Action::LongPress:
        if (currentState == State::Closed)
            notify(OpenCommandBit, NotifyAction::SetBits);

        else if (currentState == State::Warning)
        {
            motorController.openDoor();
            currentState = State::Opening;
        }
        else if (currentState == State::Closing)
        {
            currentState = State::Opening;
            notify(ErrorBit, NotifyAction::SetBits);
            motorController.revertClosing();
        }
        else if (currentState == State::WantToClose)
        {
            currentState = State::Opened;
            notify(ErrorBit, NotifyAction::SetBits);
        }
        break;

    case Button::Action::SuperLongPress:
        if (currentState == State::FatalError)
        {
            currentState = State::ManualOpening;
            motorController.manualOpen();
        }
        break;

    case Button::Action::StopLongPress:
        if (currentState == State::ManualOpening)
        {
            currentState = State::FatalError;
            motorController.disableManualMode();
        }
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::closeButtonCallback(const util::Button::Action action)
{
    switch (action)
    {
    case Button::Action::ShortPress:
        if (currentState == State::FatalError)
            currentState = State::Initializing;

        [[fallthrough]];

    case Button::Action::LongPress:
        if (currentState == State::Opened)
            notify(CloseCommandBit, NotifyAction::SetBits);

        else if (currentState == State::Warning)
        {
            if (tactileSwitches.doorSwitch.isPressing())
            {
                motorController.closeDoor();
                currentState = State::Closing;
            }
            else
            {
                currentState = State::WantToClose;
            }
        }
        else if (currentState == State::Opening)
        {
            currentState = State::Closing;
            notify(ErrorBit, NotifyAction::SetBits);
            motorController.revertOpening();
        }

        break;

    case Button::Action::SuperLongPress:
        if (currentState == State::FatalError)
        {
            currentState = State::ManualClosing;
            motorController.manualClose();
        }
        break;

    case Button::Action::StopLongPress:
        if (currentState == State::ManualClosing)
        {
            currentState = State::FatalError;
            motorController.disableManualMode();
        }
        break;

    default:
        break;
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::doorSwitchCallback(const util::Button::Action action)
{
    if (action == Button::Action::LongPress)
    {
        if (currentState == State::WantToClose)
            notify(DoorStateTriggerBit, NotifyAction::SetBits);
    }
    else if (action == Button::Action::StopLongPress && currentState == State::Closing)
    { // door wing is opening while closing the lock

        // revert closing and retry the close procedure later
        currentState = State::RetryToClose;
        notify(ErrorBit, NotifyAction::SetBits);
        motorController.revertClosing();
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::lockSwitchCallback(const util::Button::Action action)
{
    if (action == Button::Action::StopLongPress)
    { // lock switch is triggered

        if (currentState == State::Calibrating)
            notify(LockStateTriggerBit, NotifyAction::SetBits);

        else if (currentState == State::Closing)
        {
            // check position with hall value
            // stop movement?
            // save new value?
        }
    }
    else if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
    { // lock switch is released

        if (currentState == State::Calibrating)
            notify(LockStateReleaseBit, NotifyAction::SetBits);
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::motorControllerFinishedCallback(const MotorController::FailureType failureType)
{
    switch (failureType)
    {
    case MotorController::FailureType::None:
        // notify motor movement is finished successful
        if (currentState == State::Opening || //
            currentState == State::Closing || //
            currentState == State::RetryToClose)
        {
            notify(FinishedEvent, NotifyAction::SetBits);
        }
        else if (currentState == State::FatalError)
        {
            currentState = State::Initializing;
        }
        break;

    case MotorController::FailureType::CalibrationFailed:
    case MotorController::FailureType::StepperDriverNoAnswer:
        // notify failure, revoke calibration
        motorController.revokeCalibration();
        currentState = State::FatalError;
        notify(ErrorBit, NotifyAction::SetBits);
        break;

    case MotorController::FailureType::HallEncoderNoAnswer:
        if (currentState != State::Opening && currentState != State::Closing)
            [[fallthrough]];
        else
            break;

    case MotorController::FailureType::ExcessiveStepLosses:
    case MotorController::FailureType::MotorMovedExternally:
        // switch to warning, but do not revoke calibration
        currentState = State::Warning;
        notify(ErrorBit, NotifyAction::SetBits);
        break;

    case MotorController::FailureType::HallEncoderReconnected:
        // do re-calibration
        currentState = State::Initializing;
        notify(ErrorBit, NotifyAction::SetBits);
        break;
    }
}