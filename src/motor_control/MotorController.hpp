#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class MotorController : public TaskWithMemberFunctionBase
{
public:
    MotorController()
        : TaskWithMemberFunctionBase("motorControllerTask", 128, osPriorityAboveNormal3){};

protected:
    void taskMain() override;

private:
};