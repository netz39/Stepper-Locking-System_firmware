#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class StateMachine : public TaskWithMemberFunctionBase
{
public:
    StateMachine() : TaskWithMemberFunctionBase("stateMachineTask", 128, osPriorityNormal3){};

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

protected:
    void taskMain() override;

private:
    State currentState = State::Unknown;
};