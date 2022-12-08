#pragma once

#include "helpers/freertos.hpp"
#include "motor_control/MotorController.hpp"
#include "tactile_switches/TactileSwitches.hpp"
#include "wrappers/Task.hpp"

/// State machine of locking system. Handle all cases and switches inputs.
class StateMachine : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    StateMachine(TactileSwitches &tactileSwitches, MotorController &motorController,
                 void (*timeoutCallback)(TimerHandle_t xTimer))
        : TaskWithMemberFunctionBase("stateMachineTask", 128, osPriorityNormal3), //
          tactileSwitches(tactileSwitches),                                       //
          motorController(motorController),                                       //
          timeoutCallback(timeoutCallback)
    {
        tactileSwitches.openButton.setCallback(
            std::bind(&StateMachine::openButtonCallback, this, std::placeholders::_1));

        tactileSwitches.closeButton.setCallback(
            std::bind(&StateMachine::closeButtonCallback, this, std::placeholders::_1));

        tactileSwitches.doorSwitch.setCallback(
            std::bind(&StateMachine::doorSwitchCallback, this, std::placeholders::_1));

        tactileSwitches.lockSwitch.setCallback(
            std::bind(&StateMachine::lockSwitchCallback, this, std::placeholders::_1));

        motorController.setNotifyStateMaschineCallback(
            std::bind(&StateMachine::motorControllerFinishedCallback, this, std::placeholders::_1));

        tactileSwitches.forceOpen.setCallback(
            std::bind(&StateMachine::forceOpenCallback, this, std::placeholders::_1));

        tactileSwitches.forceClose.setCallback(
            std::bind(&StateMachine::forceCloseCallback, this, std::placeholders::_1));

        timeoutTimer =
            xTimerCreate("timeoutTimer", toOsTicks(1.0_min), pdFALSE, nullptr, timeoutCallback);
    };

    ~StateMachine() override = default;

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

    [[nodiscard]] State getCurrentState() const noexcept
    {
        return currentState;
    }

    void onTimeout(TimerHandle_t)
    {
        if (tactileSwitches.forceOpen.isPressing())
            openButtonCallback(util::Button::Action::ShortPress);

        else
            closeButtonCallback(util::Button::Action::ShortPress);
    }

protected:
    [[noreturn]] void taskMain() override;

private:
    TactileSwitches &tactileSwitches;
    MotorController &motorController;
    State currentState{State::Initializing};

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

    void forceOpenCallback(util::Button::Action action);
    void forceCloseCallback(util::Button::Action action);

    TimerHandle_t timeoutTimer = nullptr;
    void (*timeoutCallback)(TimerHandle_t xTimer);

    void stopTimer()
    {
        xTimerStop(timeoutTimer, 0);
    }

    void resetTimer()
    {
        xTimerReset(timeoutTimer, 0);
    }

    void motorControllerFinishedCallback(MotorController::FailureType failureType);
};