#pragma once

#include "helpers/freertos.hpp"
#include "i2c-drivers/as5600.hpp"
#include "settings/Settings.hpp"
#include "wrappers/Task.hpp"

#include <array>

using util::wrappers::TaskWithMemberFunctionBase;

/// Read the hall encoder periodically and calc the current motor position
class HallEncoder : public TaskWithMemberFunctionBase, SettingsUser
{

public:
    HallEncoder(firmwareSettings::Container &settingsContainer, i2c::RtosAccessor &busAccessor)
        : TaskWithMemberFunctionBase("hallEncoderTask", 256, osPriorityNormal3), //
          settingsContainer(settingsContainer),                                  //
          busAccessor(busAccessor)                                               //
    {
    }

    static constexpr auto EncoderResolution = 1 << 12;

    /// @return true if values are valid for further processings
    bool isOkay();

    /// get accumulated position in microsteps,
    /// crossovers (360° - 0° and vice versa ) are considered
    int32_t getPosition();

    /// get hall encoders raw position, value will be between 0 and 4095
    uint16_t getRawPosition();

    /// save the current position at home point, use it after successful homing
    /// @return true if current position can be readed and saved successfully
    bool saveHomePosition();

protected:
    void taskMain() override;

    void onSettingsUpdate() override;

private:
    firmwareSettings::Container &settingsContainer;
    i2c::RtosAccessor &busAccessor;

    AS5600::AS5600 device{busAccessor, AS5600::AS5600::Voltage::ThreePointThree,
                          AS5600::AS5600::Variant::AS5600};

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