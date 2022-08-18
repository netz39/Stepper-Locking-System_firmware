#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "util/Button.hpp"
#include "wrappers/Task.hpp"

/// All switches and button are handled here, incl. deboucing, long press detection etc
class TactileSwitches : public util::wrappers::TaskWithMemberFunctionBase
{
public:
    TactileSwitches()
        : TaskWithMemberFunctionBase("switchesPollingTask", 128, osPriorityBelowNormal3){};
    ~TactileSwitches() override = default;

    /// press event when triggering button
    util::Button openButton{{OpenButton_GPIO_Port, OpenButton_Pin}, 250.0_ms, true};

    /// press event when triggering button
    util::Button closeButton{{CloseButton_GPIO_Port, CloseButton_Pin}, 250.0_ms, true};

    /// press event when triggering (normally closed) switch
    util::Button doorSwitch{{DoorState_GPIO_Port, DoorState_Pin}, 500.0_ms, false};

    /// press event when releasing switch (normally closed)
    /// StopLongPress event when triggering switch
    util::Button lockSwitch{{LockState_GPIO_Port, LockState_Pin}, 30.0_ms, true};

    // force open wire will pulled down when the door should open
    util::Button forceOpen{{ForceOpenInput_GPIO_Port, ForceOpenInput_Pin}, 100.0_ms, false};

    // force close wire will pulled down when the door should close
    util::Button forceClose{{ForceCloseInput_GPIO_Port, ForceCloseInput_Pin}, 100.0_ms, false};

protected:
    [[noreturn]] void taskMain() override;
};