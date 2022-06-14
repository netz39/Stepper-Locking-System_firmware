#pragma once

#include "main.h"

#include "helpers/freertos.hpp"
#include "util/Button.hpp"
#include "wrappers/Task.hpp"

using util::wrappers::TaskWithMemberFunctionBase;

class TactileSwitches : public TaskWithMemberFunctionBase
{
public:
    static constexpr auto DebouncingPeriod = 50.0_ms; // to filter bouncings and odd spikes
    static constexpr auto LongPressTime = 0.5_s;

    TactileSwitches() : TaskWithMemberFunctionBase("switchesPollingTask", 128, osPriorityLow5){};

protected:
    void taskMain() override;

private:
    void openButtonCallback(util::Button::Action action);
    void closeButtonCallback(util::Button::Action action);
    void doorSwitchCallback(util::Button::Action action);
    void lockSwitchCallback(util::Button::Action action);

    util::Button openButton{
        {OpenButton_GPIO_Port, OpenButton_Pin},
        std::bind(&TactileSwitches::openButtonCallback, this, std::placeholders::_1),
    };

    util::Button closeButton{
        {CloseButton_GPIO_Port, CloseButton_Pin},
        std::bind(&TactileSwitches::closeButtonCallback, this, std::placeholders::_1)};

    util::Button doorSwitch{
        {DoorState_GPIO_Port, DoorState_Pin},
        std::bind(&TactileSwitches::doorSwitchCallback, this, std::placeholders::_1),
        true};

    util::Button lockSwitch{
        {LockState_GPIO_Port, LockState_Pin},
        std::bind(&TactileSwitches::lockSwitchCallback, this, std::placeholders::_1),
        true};
};