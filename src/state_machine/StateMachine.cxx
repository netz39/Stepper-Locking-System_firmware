#include "StateMachine.hpp"
#include "sync.hpp"

#include <climits>

using util::Button;

void StateMachine::taskMain()
{
    // wait some time to get steady switches states
    vTaskDelay(toOsTicks(500.0_ms));

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
            waitForCloseCommand();

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
            waitForOpenCommand();

            motorController.openDoor();
            currentState = State::Opening;
        }
        break;

        //------------------------------
        case State::Opening:
        {
            waitForFinishedEvent();

            if (stateChanged)
            {
                stateChanged = false;
                break;
            }

            currentState = State::Opened;
        }
        break;

        //------------------------------
        case State::Closing:
        {
            waitForFinishedEvent();

            if (stateChanged)
            {
                stateChanged = false;
                break;
            }

            currentState = State::Closed;
            break;
        }

        //------------------------------
        case State::WantToClose:
        {
            waitForDoorStateTriggered();

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
        case State::Calibrating:
        {
            if (!tactileSwitches.lockSwitch.isLongPressing())
            { // lock switch already triggered, so opens the door until the lock switch is released

                motorController.doCalibration(true);
                waitForLockStateReleased();
                motorController.abortCalibration();
            }

            motorController.doCalibration();
            waitForLockStateTriggered();
            motorController.calibrationIsDone();
            isCalibrated = true;
            currentState = State::Closed;
        }
        break;

        //------------------------------
        case State::Warning:
            break;

        //------------------------------
        default:
        case State::FatalError:
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
            if ((notifiedValue & eventBit) != 0)
                return true;
        }
        else
            // timeout occurred
            return false;
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForOpenCommand()
{
    waitForCommand(OpenCommandBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForCloseCommand()
{
    waitForCommand(CloseCommandBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForDoorStateTriggered()
{
    if (tactileSwitches.doorSwitch.isLongPressing())
        return;

    waitForCommand(DoorStateTriggerBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForLockStateTriggered()
{
    if (!tactileSwitches.lockSwitch.isLongPressing())
        return;

    waitForCommand(LockStateTriggerBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForLockStateReleased()
{
    if (tactileSwitches.lockSwitch.isLongPressing())
        return;

    waitForCommand(LockStateReleaseBit, portMAX_DELAY);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForFinishedEvent()
{
    waitForCommand(FinishedEvent, portMAX_DELAY);
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
            stateChanged = true;
            motorControllerFinishedCallback(); // invoke state change
            motorController.revertClosing();
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
            stateChanged = true;
            motorControllerFinishedCallback(); // invoke state change
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
    {
        currentState = State::Opening;
        stateChanged = true;
        motorControllerFinishedCallback(); // invoke state change
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
void StateMachine::motorControllerFinishedCallback()
{
    if (currentState == State::Opening || currentState == State::Closing)
        notify(FinishedEvent, eSetBits);
}