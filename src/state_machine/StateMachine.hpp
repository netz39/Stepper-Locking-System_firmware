#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "motor_control/MotorController.hpp"
#include "tactile_switches/TactileSwitches.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class StateMachine : public TaskWithMemberFunctionBase
{
public:
    StateMachine(TactileSwitches &tactileSwitches, MotorController &motorController)
        : TaskWithMemberFunctionBase("stateMachineTask", 128, osPriorityNormal3), //
          tactileSwitches(tactileSwitches),                                       //
          motorController(motorController)
    {
        tactileSwitches.openButton.setCallback(
            std::bind(&StateMachine::openButtonCallback, this, std::placeholders::_1));

        tactileSwitches.closeButton.setCallback(
            std::bind(&StateMachine::closeButtonCallback, this, std::placeholders::_1));

        tactileSwitches.doorSwitch.setCallback(
            std::bind(&StateMachine::doorSwitchCallback, this, std::placeholders::_1));

        tactileSwitches.lockSwitch.setCallback(
            std::bind(&StateMachine::lockSwitchCallback, this, std::placeholders::_1));
    };

    enum class State
    {
        Unknown,
        Opened,
        Closed,
        Opening,
        Closing,
        WantToClose,
        Calibrating,
        Warning,
        FatalError
    };

    static constexpr uint32_t OpenCommandBit = 1 << 2;
    static constexpr uint32_t CloseCommandBit = 1 << 3;
    static constexpr uint32_t DoorStateTriggerBit = 1 << 4;
    static constexpr uint32_t LockStateTriggerBit = 1 << 5;

    State currentState = State::Unknown;

protected:
    void taskMain() override;

private:
    TactileSwitches &tactileSwitches;
    MotorController &motorController;

    void waitForOpenCommand();
    void waitForCloseCommand();
    void waitForDoorStateTriggered();
    void waitForLockStateTriggered();

    bool listenForOpenButton = false;
    bool listenForCloseButton = false;
    bool listenForDoorSwitch = false;
    bool listenForLockSwitch = false;

    void openButtonCallback(util::Button::Action action);
    void closeButtonCallback(util::Button::Action action);
    void doorSwitchCallback(util::Button::Action action);
    void lockSwitchCallback(util::Button::Action action);
};