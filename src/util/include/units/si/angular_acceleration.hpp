#pragma once

#include "units/si.hpp"

namespace units::si
{
using AngularAcceleration = Value<SiUnit<0, 0, -2, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::AngularAcceleration operator"" _rad_s2(const long double magnitude)
{
    return units::si::AngularAcceleration(magnitude);
}
