#pragma once

#include "main.h"
#include "spi.h"
#include "tim.h"

#include "BgrColor.hpp"
#include "GammaCorrection.hpp"
#include "util/PwmLed.hpp"
#include "wrappers/Task.hpp"

#include <array>

using util::PwmOutput8Bit;
using util::pwm_led::DualLed;
using util::wrappers::TaskWithMemberFunctionBase;

class LightController : public TaskWithMemberFunctionBase
{
public:
    LightController() : TaskWithMemberFunctionBase("statusLedTask", 512, osPriorityLow4)
    {
        endFrames.fill(0xFF);
    }

    static constexpr PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1};
    static constexpr PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1};
    DualLed<uint8_t> statusLed{RedChannel, GreenChannel};

    static constexpr auto SpiDevice = &hspi1;

    struct LedSpiData
    {
        uint8_t Start = 0xFF;
        BgrColor color;

        void assignGammaCorrectedColor(BgrColor newColor)
        {
            color.blue = GammaCorrectionLUT[newColor.blue];
            color.green = GammaCorrectionLUT[newColor.green];
            color.red = GammaCorrectionLUT[newColor.red];
        }
    };

    void notifySpiIsFinished();

protected:
    void taskMain() override;

private:
    static constexpr auto NumberOfLedsInRing = 32;
    static constexpr auto NumberOfLedsInCrossbar = 10;
    static constexpr auto TotalNumberOfLeds = NumberOfLedsInRing + NumberOfLedsInCrossbar;
    static constexpr auto NumberOfEndFrames = (TotalNumberOfLeds + 15) / 16;

    std::array<BgrColor, TotalNumberOfLeds> ledSegments;
    std::array<LedSpiData, TotalNumberOfLeds> ledSpiDatas;
    std::array<uint8_t, NumberOfEndFrames> endFrames;

    void sendStartFrame();
    void convertToGammaCorrectedColors();
    void sendBuffer();
};