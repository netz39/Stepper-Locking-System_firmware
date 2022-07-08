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

    bool isEncoderOkay();

    /// get accumulated position in microsteps,
    /// crossovers (360° - 0° and vice versa ) are considered
    int32_t getPosition();

    /// get hall encoders raw position, value will be between 0 and 4095
    uint16_t getRawPosition();

    /// set the position in microsteps, use it after successful homing
    void setPosition(int32_t microsteps);

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

    uint16_t prevHallEncoderRawValue = 0;
    float accumulatedPosition = 0.0f;
    bool isIncrementingAtOpening = false;
};