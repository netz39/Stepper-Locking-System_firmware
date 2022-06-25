#pragma once

#include "main.h"
#include "spi.h"
#include "tim.h"

#include "LedDataTypes.hpp"
#include "state_machine/StateMachine.hpp"
#include "util/PwmLed.hpp"
#include "wrappers/Task.hpp"

#include "AnimationController.hpp"

#include <array>

using util::PwmOutput8Bit;
using util::pwm_led::DualLed;
using util::wrappers::TaskWithMemberFunctionBase;

class LightController : public TaskWithMemberFunctionBase
{
public:
    LightController(StateMachine &stateMaschine)
        : TaskWithMemberFunctionBase("lightControllerTask", 512, osPriorityLow4), //
          stateMaschine(stateMaschine)
    {
        endFrames.fill(0xFF);
    }

    static constexpr PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1};
    static constexpr PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1};
    DualLed<uint8_t> statusLed{RedChannel, GreenChannel};

    static constexpr auto SpiDevice = &hspi1;

    void notifySpiIsFinished();

protected:
    void taskMain() override;

private:
    static constexpr auto NumberOfEndFrames = (NumberOfRings * NumberOfLedsPerRing + 15) / 16;

    LedSegmentArray ledSegments1;
    LedSpiDataArray ledSpiData1;
    LedSegmentArray ledSegments2;
    LedSpiDataArray ledSpiData2;
    std::array<uint8_t, NumberOfEndFrames> endFrames;

    DoorIsOpenAnimation doorIsOpenAnimation{ledSegments1};
    DoorIsClosedAnimation doorIsClosedAnimation{ledSegments1};
    DoorShouldCloseAnimation doorShouldCloseAnimation{ledSegments1};
    WhirlingAnimation whirlingAnimation{ledSegments1};
    RainbowAnimation rainbowAnimation{ledSegments1};

    ShowStatusAnimation showStatusAnimation{ledSegments1};
    TestAllColorsAnimation testAllColorsAnimation{ledSegments1};

    LedAnimationBase *targetAnimation{&showStatusAnimation};

    void sendStartFrame();

    /// convert LED data to gamma corrected colors and put it to SPI-related array
    void convertToGammaCorrectedColors(LedSegmentArray &source, LedSpiDataArray &destination);

    void sendBuffer();
    void updateLightState();

    StateMachine &stateMaschine;
    StateMachine::State prevState = StateMachine::State::Unknown;
};