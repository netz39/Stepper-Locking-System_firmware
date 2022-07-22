#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "motor_control/MotorController.hpp"
#include "tactile_switches/TactileSwitches.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

/// State machine of locking system. Handle all cases and switches inputs.
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

        motorController.setFinishedCallback(
            std::bind(&StateMachine::motorControllerFinishedCallback, this, std::placeholders::_1));
    };

    enum class State
    {
        Initializing,
        Opened,
        Closed,
        Opening,
        Closing,
        ManualOpening,
        ManualClosing,
        WantToClose,
        RetryToClose,
        Calibrating,
        Warning,
        FatalError
    };

    static constexpr uint32_t ErrorBit = 1 << 1;
    static constexpr uint32_t OpenCommandBit = 1 << 2;
    static constexpr uint32_t CloseCommandBit = 1 << 3;
    static constexpr uint32_t DoorStateTriggerBit = 1 << 4;
    static constexpr uint32_t LockStateTriggerBit = 1 << 5;
    static constexpr uint32_t LockStateReleaseBit = 1 << 6;
    static constexpr uint32_t FinishedEvent = 1 << 7;

    State currentState = State::Initializing;

protected:
    [[noreturn]] void taskMain() override;

private:
    TactileSwitches &tactileSwitches;
    MotorController &motorController;

    bool isCalibrated = false;

    bool waitForCommand(uint32_t eventBit, TickType_t xTicksToWait);

    bool waitForOpenCommand();
    bool waitForCloseCommand();
    bool waitForDoorStateTriggered();
    bool waitForLockStateTriggered();
    bool waitForLockStateReleased();
    bool waitForFinishedEvent();

    void openButtonCallback(util::Button::Action action);
    void closeButtonCallback(util::Button::Action action);
    void doorSwitchCallback(util::Button::Action action);
    void lockSwitchCallback(util::Button::Action action);

    void motorControllerFinishedCallback(MotorController::FailureType failureType);
};