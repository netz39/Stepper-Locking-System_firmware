#pragma once

#include "as5600.hpp"
#include "helpers/freertos.hpp"
#include "settings/Settings.hpp"
#include "wrappers/Task.hpp"

#include <array>

/// Read the hall encoder periodically and calc the current motor position
class HallEncoder : public util::wrappers::TaskWithMemberFunctionBase, SettingsUser
{

public:
    HallEncoder(const firmwareSettings::Container &settingsContainer, I2cAccessor &busAccessor)
        : TaskWithMemberFunctionBase("hallEncoderTask", 256, osPriorityNormal3), //
          settingsContainer(settingsContainer),                                  //
          busAccessor(busAccessor)                                               //
    {
    }
    ~HallEncoder() override = default;

    static constexpr auto EncoderResolution = 1 << 12;

    /// @return true if values are valid for further processings
    [[nodiscard]] bool isOkay() const;

    /// get accumulated position in microsteps,
    /// crossovers (360° - 0° and vice versa ) are considered
    [[nodiscard]] int32_t getPosition() const;

    /// get hall encoders raw position, value will be between 0 and 4095
    [[nodiscard]] uint16_t getRawPosition() const;

    /// save the current position at home point, use it after successful homing
    /// @return true if current position can be readed and saved successfully
    bool saveHomePosition();

protected:
    [[noreturn]] void taskMain() override;

    void onSettingsUpdate() override;

private:
    const firmwareSettings::Container &settingsContainer;
    I2cAccessor &busAccessor;

    AS5600 device{busAccessor, AS5600::Voltage::ThreePointThree, AS5600::Variant::AS5600};

    void configureHall();

    /// get difference to previous time step and detect cross overs and convert to mircosteps
    void calculatePosition();

    /// crossovers (360° - 0° and vice versa ) will be detected and inc/dec the current turn
    void detectCrossovers();

    bool isIncrementingAtOpening = false;
    uint16_t prevHallEncoderRawValue = 0;
    int32_t currentPosition = 0;
    int32_t currentTurn = 0;
    uint16_t encoderValueAtHomePoint = 0;
};