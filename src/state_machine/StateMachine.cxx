#include "StateMachine.hpp"
#include <climits>

using util::Button;

void StateMachine::taskMain()
{
    // wait some time to get steady switches states
    vTaskDelay(toOsTicks(500.0_ms));

    while (true)
    {
        switch (currentState)
        {
        case State::Unknown:
        {
            if (tactileSwitches.doorSwitch.isLongPressing())
            {
                // door wing is triggering doorSwitch

                if (!tactileSwitches.lockSwitch.isLongPressing())
                {
                    // lock switch is pressed -> lock bolt is extended
                    // ToDo: compare with saved hall encoder value

                    currentState = State::Closed;
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
                currentState = State::Opened;
            }
        }
        break;

        case State::Opened:
        {
            waitForCloseCommand();

            if (tactileSwitches.doorSwitch.isLongPressing())
            {
                // door wing is triggering doorSwitch
                motorController.closeDoor();
                currentState = State::Closing;
            }
            else
            {
                // door wing not closed, keep close command in mind
                currentState = State::WantToClose;
            }
        }
        break;

        case State::Closed:
        {
            waitForOpenCommand();

            motorController.openDoor();
            currentState = State::Opening;
        }
        break;

        case State::Opening:
        {
            while (motorController.isRunning())
                vTaskDelay(toOsTicks(100.0_Hz));

            currentState = State::Opened;
        }
        break;

        case State::Closing:
            while (motorController.isRunning())
                vTaskDelay(toOsTicks(100.0_Hz));

            currentState = State::Closed;
            break;

        case State::WantToClose:
        {
            waitForDoorStateTriggered();

            motorController.closeDoor();
            currentState = State::Closing;
        }
        break;

        case State::Calibrating:
        {
            motorController.doCalibration();

            waitForLockStateTriggered();

            motorController.calibrationIsDone();
            // ToDo: save actual hall encoder value

            currentState = State::Closed;
        }
        break;

        case State::Warning:
            break;

        default:
        case State::FatalError:
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForOpenCommand()
{
    while (true)
    {
        listenForOpenButton = true;

        uint32_t notifiedValue;
        notifyWait(ULONG_MAX, ULONG_MAX, &notifiedValue, portMAX_DELAY);

        if ((notifiedValue & OpenCommandBit) != 0)
        {
            listenForOpenButton = false;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForCloseCommand()
{
    while (true)
    {
        listenForCloseButton = true;

        uint32_t notifiedValue;
        notifyWait(ULONG_MAX, ULONG_MAX, &notifiedValue, portMAX_DELAY);

        if ((notifiedValue & CloseCommandBit) != 0)
        {
            listenForCloseButton = false;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForDoorStateTriggered()
{
    if (tactileSwitches.doorSwitch.isLongPressing())
        return;

    while (true)
    {
        listenForDoorSwitch = true;

        uint32_t notifiedValue;
        notifyWait(ULONG_MAX, ULONG_MAX, &notifiedValue, portMAX_DELAY);

        if ((notifiedValue & DoorStateTriggerBit) != 0)
        {
            listenForDoorSwitch = false;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::waitForLockStateTriggered()
{
    if (!tactileSwitches.lockSwitch.isLongPressing())
        return;

    while (true)
    {
        listenForLockSwitch = true;

        uint32_t notifiedValue;
        notifyWait(ULONG_MAX, ULONG_MAX, &notifiedValue, portMAX_DELAY);

        if ((notifiedValue & LockStateTriggerBit) != 0)
        {
            listenForLockSwitch = false;
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
void StateMachine::openButtonCallback(util::Button::Action action)
{
    if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
        if (listenForOpenButton)
            notify(OpenCommandBit, eSetBits);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::closeButtonCallback(util::Button::Action action)
{
    if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
        if (listenForCloseButton)
            notify(CloseCommandBit, eSetBits);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::doorSwitchCallback(util::Button::Action action)
{
    if (action == Button::Action::ShortPress || action == Button::Action::LongPress)
        if (listenForDoorSwitch)
            notify(DoorStateTriggerBit, eSetBits);
}

//--------------------------------------------------------------------------------------------------
void StateMachine::lockSwitchCallback(util::Button::Action action)
{
    if (action == Button::Action::StopLongPress)
        if (listenForLockSwitch)
            notify(LockStateTriggerBit, eSetBits);
}