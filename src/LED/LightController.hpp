#pragma once

#include "cmsis_os2.h"
#include "main.h"
#include "tim.h"

#include "util/PwmLed.hpp"
#include "wrappers/Task.hpp"

#include <array>

using util::PwmOutput8Bit;
using util::pwm_led::DualLed;
using util::wrappers::TaskWithMemberFunctionBase;

class LightController : public TaskWithMemberFunctionBase
{
public:
    LightController() : TaskWithMemberFunctionBase("statusLedTask", 128, osPriorityLow4){};

    static constexpr PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1};
    static constexpr PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1};
    DualLed<uint8_t> statusLed{RedChannel, GreenChannel};

protected:
    void taskMain() override;
};