#pragma once

// todo handles should be inserted via constructor to make class testable
#include "main.h"
#include "spi.h"
#include "tim.h"

#include "AddressableLedDriver.hpp"
#include "state_machine/StateMachine.hpp"
#include "util/PwmLed.hpp"
#include "wrappers/Task.hpp"

#include "AnimationController.hpp"

#include <array>

/// Controls the status LED and the addressable LED rings
class LightController : public util::wrappers::TaskWithMemberFunctionBase, SettingsUser
{
public:
    LightController(SPI_HandleTypeDef *SpiDevice,
                    const firmwareSettings::Container &settingsContainer,
                    const StateMachine &stateMachine, const MotorController &motorController)
        : TaskWithMemberFunctionBase("lightControllerTask", 512, osPriorityLow4),
          ledDriver(SpiDevice),                 //
          settingsContainer(settingsContainer), //
          stateMachine(stateMachine),           //
          motorController(motorController){};

    ~LightController() override = default;

    static constexpr util::PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1}; // todo handle should be given via constructor
    static constexpr util::PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1}; // todo handle should be given via constructor

    void notifySpiIsFinished();

protected:
    [[noreturn]] void taskMain() override;
    void onSettingsUpdate() override;

private:
    util::pwm_led::DualLed<uint8_t> statusLed{RedChannel, GreenChannel};

    AddressableLedDriver ledDriver;
    LedSegmentArray ledSegments1{};
    LedSegmentArray ledSegments2{};

    bool invertRotationDirection = false;

    DualAnimations<DoorIsOpenAnimation> doorIsOpenAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorIsClosedAnimation> doorIsClosedAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorShouldCloseAnimation> doorShouldCloseAnimation{ledSegments1, ledSegments2};
    DualAnimations<WhirlingAnimation> whirlingAnimation{ledSegments1, ledSegments2};
    DualAnimations<ShowStatusAnimation> showStatusAnimation{ledSegments1, ledSegments2};

    LedAnimationBase *targetAnimation{&showStatusAnimation};

    void updateLightState();

    const firmwareSettings::Container &settingsContainer;
    const StateMachine &stateMachine;
    StateMachine::State prevState = StateMachine::State::Initializing;

    const MotorController &motorController;
};