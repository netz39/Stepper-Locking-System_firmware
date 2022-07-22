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

/// Controls the status LED and the addressable LED rings
class LightController : public TaskWithMemberFunctionBase, SettingsUser
{
public:
    LightController(const firmwareSettings::Container &settingsContainer, const StateMachine &stateMaschine,
                    const MotorController &motorController)
        : TaskWithMemberFunctionBase("lightControllerTask", 512, osPriorityLow4), //
          settingsContainer(settingsContainer),                                   //
          stateMaschine(stateMaschine),                                           //
          motorController(motorController)

    {
        endFrames.fill(0xFF);
    }

    static constexpr PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1};
    static constexpr PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1};
    DualLed<uint8_t> statusLed{RedChannel, GreenChannel};

    static constexpr auto SpiDevice = &hspi1;

    void notifySpiIsFinished();

protected:
    [[noreturn]] void taskMain() override;
    void onSettingsUpdate() override;

private:
    static constexpr auto NumberOfEndFrames = (NumberOfRings * NumberOfLedsPerRing + 15) / 16;

    bool invertRotationDirection = false;

    LedSegmentArray ledSegments1;
    LedSpiDataArray ledSpiData1;
    LedSegmentArray ledSegments2;
    LedSpiDataArray ledSpiData2;
    std::array<uint8_t, NumberOfEndFrames> endFrames{};

    DualAnimations<DoorIsOpenAnimation> doorIsOpenAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorIsClosedAnimation> doorIsClosedAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorShouldCloseAnimation> doorShouldCloseAnimation{ledSegments1, ledSegments2};
    DualAnimations<WhirlingAnimation> whirlingAnimation{ledSegments1, ledSegments2};
    DualAnimations<ShowStatusAnimation> showStatusAnimation{ledSegments1, ledSegments2};

    LedAnimationBase *targetAnimation{&showStatusAnimation};

    void sendStartFrame();

    /// convert LED data to gamma corrected colors and put it to SPI-related array
    void convertToGammaCorrectedColors(LedSegmentArray &source, LedSpiDataArray &destination);

    void sendBuffer();
    void updateLightState();

    const firmwareSettings::Container &settingsContainer;
    const StateMachine &stateMaschine;
    StateMachine::State prevState = StateMachine::State::Initializing;

    const MotorController &motorController;
};