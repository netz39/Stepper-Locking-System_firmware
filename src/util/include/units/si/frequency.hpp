#pragma once

#include "units/si.hpp"

namespace units::si
{
using Frequency = Value<SiUnit<0, 0, -1, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Frequency operator""_Hz(const long double magnitude)
{
    return units::si::Frequency(magnitude);
}
