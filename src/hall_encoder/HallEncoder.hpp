#pragma once

#include "helpers/freertos.hpp"
#include "i2c-drivers/as5600.hpp"
#include "wrappers/Task.hpp"

#include <array>

using util::wrappers::TaskWithMemberFunctionBase;

class HallEncoder : public TaskWithMemberFunctionBase
{

public:
    static constexpr auto TaskFrequency = 100.0_Hz;

    HallEncoder(i2c::RtosAccessor &busAccessor)
        : TaskWithMemberFunctionBase("hallEncoderTask", 128, osPriorityNormal3), //
          busAccessor(busAccessor)                                               //
    {
    }

protected:
    void taskMain() override;

private:
    i2c::RtosAccessor &busAccessor;

    void configureHall();

public:
    AS5600::AS5600 device{busAccessor, AS5600::AS5600::Voltage::ThreePointThree,
                          AS5600::AS5600::Variant::AS5600, false};
};