#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class StateMachine : public TaskWithMemberFunctionBase
{
public:
    StateMachine() : TaskWithMemberFunctionBase("stateMachineTask", 128, osPriorityNormal3){};

protected:
    void taskMain() override;

private:
};