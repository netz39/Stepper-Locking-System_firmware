#include "StateMachine.hpp"
#include "sync.hpp"

#include <climits>

using util::Button;

void StateMachine::taskMain()
{
    // wait some time to get steady switches states
    vTaskDelay(toOsTicks(550.0_ms));

    sync::waitForAll(sync::ConfigurationLoaded);

    while (true)
    {
        switch (currentState)
        {

        //------------------------------
        case State::Unknown:
        {
            if (tactileSwitches.doorSwitch.isLongPressing())
            {
                // door wing is triggering doorSwitch

                if (!tactileSwitches.lockSwitch.isLongPressing())
                {
                    // lock switch is pressed -> lock bolt is extended
                    // ToDo: compare with saved hall encoder value
                    // if that matchs -> set to closed, otherwise recalibrate
                    // currentState = State::Closed;

                    currentState = State::Calibrating;
                }
                else
                {
                    // lock switch is not triggered - recalibration needed
                    currentState = State::Calibrating;
                }
            }
            else
            {
                // door wing is open, so we assume door and lock bolt are correctly open
                // we will calibrate later at the moment, when the door is closing
                currentState = State::Opened;
            }
        }
        break;

        //------------------------------
        case State::Opened:
        {
            if (!waitForCloseCommand())
                break;

            if (tactileSwitches.doorSwitch.isLongPressing())
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
            if (!tactileSwitches.lockSwitch.isLongPressing())
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
        case State::FatalError:
            motorController.freezeMotor();
            vTaskDelay(toOsTicks(20.0_ms));
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForCommand(uint32_t eventBit, TickType_t xTicksToWait)
{
    while (true)
    {
        uint32_t notifiedValue;
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
    return waitForCommand(OpenCommandBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForCloseCommand()
{
    return waitForCommand(CloseCommandBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForDoorStateTriggered()
{
    if (tactileSwitches.doorSwitch.isLongPressing())
        return true;

    return waitForCommand(DoorStateTriggerBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForLockStateTriggered()
{
    if (!tactileSwitches.lockSwitch.isLongPressing())
        return true;

    return waitForCommand(LockStateTriggerBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForLockStateReleased()
{
    if (tactileSwitches.lockSwitch.isLongPressing())
        return true;

    return waitForCommand(LockStateReleaseBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
bool StateMachine::waitForFinishedEvent()
{
    return waitForCommand(FinishedEvent, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::openButtonCallback(util::Button::Action action)
{
    if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
    {
        if (currentState == State::Closed)
            notify(OpenCommandBit, eSetBits);

        else if (currentState == State::Closing)
        {
            currentState = State::Opening;
            notify(ErrorBit, eSetBits);
            motorController.revertClosing();
        }
        else if (currentState == State::WantToClose)
        {
            currentState = State::Opened;
            notify(ErrorBit, eSetBits);
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::closeButtonCallback(util::Button::Action action)
{
    if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
    {
        if (currentState == State::Opened)
            notify(CloseCommandBit, eSetBits);

        else if (currentState == State::Opening)
        {
            currentState = State::Closing;
            notify(ErrorBit, eSetBits);
            motorController.revertOpening();
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::doorSwitchCallback(util::Button::Action action)
{
    if (action == Button::Action::LongPress)
    {
        if (currentState == State::WantToClose)
            notify(DoorStateTriggerBit, eSetBits);
    }
    else if (action == Button::Action::StopLongPress && currentState == State::Closing)
    { // door wing is opening while closing the lock

        // revert closing and retry the close procedure later
        currentState = State::RetryToClose;
        notify(ErrorBit, eSetBits);
        motorController.revertClosing();
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::lockSwitchCallback(util::Button::Action action)
{
    if (action == Button::Action::StopLongPress)
    { // lock switch is triggered

        if (currentState == State::Calibrating)
            notify(LockStateTriggerBit, eSetBits);

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
            notify(LockStateReleaseBit, eSetBits);
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::motorControllerFinishedCallback(bool success)
{
    if (success)
    {
        if (currentState == State::Opening || currentState == State::Closing ||
            currentState == State::RetryToClose)
            notify(FinishedEvent, eSetBits);
    }
    else
    {
        notify(ErrorBit, eSetBits);
        currentState = State::FatalError;
    }
}