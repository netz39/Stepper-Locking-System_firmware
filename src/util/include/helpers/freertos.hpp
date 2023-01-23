#pragma once

#include "units/si/frequency.hpp"
#include "units/si/scalar.hpp"
#include "units/si/time.hpp"
#include <FreeRTOS.h>
#include <cstdint>

constexpr uint32_t toMilliSeconds(const units::si::Time time)
{
    return time.getMagnitude<uint32_t>(units::si::scale::ms);
}

constexpr uint32_t toMilliSeconds(const units::si::Frequency frequency)
{
    return toMilliSeconds(1_ / frequency);
}

constexpr TickType_t toOsTicks(const units::si::Time time)
{
    return toMilliSeconds(time) * (configTICK_RATE_HZ / 1000.0);
}

constexpr TickType_t toOsTicks(const units::si::Frequency frequency)
{
    return toOsTicks(1_ / frequency);
}

constexpr units::si::Time ticksToTime(const TickType_t ticks)
{
    return units::si::Time{static_cast<float>(ticks) / configTICK_RATE_HZ};
}