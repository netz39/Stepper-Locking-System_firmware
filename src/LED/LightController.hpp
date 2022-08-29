#pragma once

// todo handles should be inserted via constructor to make class testable
#include "main.h"
#include "spi.h"
#include "tim.h"

#include "GammaCorrection.hpp"
#include "LedDataTypes.hpp"
#include "state_machine/StateMachine.hpp"
#include "util/PwmLed.hpp"
#include "wrappers/Task.hpp"

#include "AnimationController.hpp"

#include <array>

// todo split class into driver and control

/// Controls the status LED and the addressable LED rings
class LightController : public util::wrappers::TaskWithMemberFunctionBase, SettingsUser
{
public:
    LightController(const firmwareSettings::Container &settingsContainer, const StateMachine &stateMachine,
                    const MotorController &motorController)
        : TaskWithMemberFunctionBase("lightControllerTask", 512, osPriorityLow4), //
          settingsContainer(settingsContainer),                                   //
          stateMachine(stateMachine),                                           //
          motorController(motorController)

    {
        endFrames.fill(0xFF);
    }
    ~LightController() override = default;

    static constexpr util::PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1}; // todo handle should be given via constructor
    static constexpr util::PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1}; // todo handle should be given via constructor

    void notifySpiIsFinished();

    [[nodiscard]] SPI_HandleTypeDef & getSPIPeripheral() noexcept {
        return SpiDevice;
    }

protected:
    [[noreturn]] void taskMain() override;
    void onSettingsUpdate() override;

private:
    struct LedSpiData
    {
        uint8_t Start = 0xFF; //!< the first byte contains control data like brightness
        BgrColor color;

        void assignGammaCorrectedColor(BgrColor newColor)
        {
            color.blue = GammaCorrectionLUT[newColor.blue];
            color.green = GammaCorrectionLUT[newColor.green];
            color.red = GammaCorrectionLUT[newColor.red];
        }
    };

    /// array, in which its content is directly sended over SPI - contains control data
    using LedSpiDataArray = std::array<LedSpiData, NumberOfLedsPerRing>;

    util::pwm_led::DualLed<uint8_t> statusLed{RedChannel, GreenChannel};
    SPI_HandleTypeDef & SpiDevice = hspi1; // todo handle should be given via constructor

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
    const StateMachine &stateMachine;
    StateMachine::State prevState = StateMachine::State::Initializing;

    const MotorController &motorController;
};