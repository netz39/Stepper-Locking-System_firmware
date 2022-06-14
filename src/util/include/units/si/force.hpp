#pragma once

#include "units/si.hpp"

namespace units::si
{
using Force = Value<SiUnit<1, 1, -2, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Force operator""_N(const long double magnitude)
{
    return units::si::Force(magnitude);
}
