#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "util/Button.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class TactileSwitches : public TaskWithMemberFunctionBase
{
public:
    TactileSwitches()
        : TaskWithMemberFunctionBase("switchesPollingTask", 128, osPriorityBelowNormal3){};

    /// press event when triggering button
    util::Button openButton{{OpenButton_GPIO_Port, OpenButton_Pin}, 30.0_ms, true};

    /// press event when triggering button
    util::Button closeButton{{CloseButton_GPIO_Port, CloseButton_Pin}, 30.0_ms, true};

    /// press event when triggering (normally closed) switch
    util::Button doorSwitch{{DoorState_GPIO_Port, DoorState_Pin}, 250.0_ms, false};

    /// press event when releasing switch - StopLongPress event when triggering switch
    util::Button lockSwitch{{LockState_GPIO_Port, LockState_Pin}, 30.0_ms, true};

protected:
    void taskMain() override;
};