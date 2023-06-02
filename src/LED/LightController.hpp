#pragma once

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
    LightController(SPI_HandleTypeDef *SpiDevice, util::pwm_led::DualLed<uint8_t> &statusLed,
                    const firmwareSettings::Container &settingsContainer,
                    const StateMachine &stateMachine, const MotorController &motorController)
        : TaskWithMemberFunctionBase("lightControllerTask", 512, osPriorityLow4),
          ledDriver(SpiDevice),                 //
          statusLed(statusLed),                 //
          settingsContainer(settingsContainer), //
          stateMachine(stateMachine),           //
          motorController(motorController){};

    ~LightController() override = default;

    void notifySpiIsFinished();

protected:
    [[noreturn]] void taskMain() override;
    void onSettingsUpdate() override;

private:
    AddressableLedDriver ledDriver;
    LedSegmentArray ledSegments1{};
    LedSegmentArray ledSegments2{};

    util::pwm_led::DualLed<uint8_t> &statusLed;
    const firmwareSettings::Container &settingsContainer;
    const StateMachine &stateMachine;
    StateMachine::State prevState = StateMachine::State::Initializing;

    const MotorController &motorController;

    bool invertRotationDirection = false;
    bool showRainbow = false;

    DualAnimations<DoorIsOpenAnimation> doorIsOpenAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorIsClosedAnimation> doorIsClosedAnimation{ledSegments1, ledSegments2};
    DualAnimations<DoorShouldCloseAnimation> doorShouldCloseAnimation{ledSegments1, ledSegments2};
    DualAnimations<WhirlingAnimation> whirlingAnimation{ledSegments1, ledSegments2};
    DualAnimations<ShowStatusAnimation> showStatusAnimation{ledSegments1, ledSegments2};
    DualAnimations<RainbowAnimation> rainbowAnimation{ledSegments1, ledSegments2};

    LedAnimationBase *targetAnimation{&showStatusAnimation};

    void updateLightState();
    void insertRainbow();
};