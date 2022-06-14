#pragma once

#include "units/si.hpp"

namespace units::si
{
using AngularVelocity = Value<SiUnit<0, 0, -1, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::AngularVelocity operator"" _rad_s(const long double magnitude)
{
    return units::si::AngularVelocity(magnitude);
}
