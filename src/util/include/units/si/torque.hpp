#pragma once

#include "units/si.hpp"

namespace units::si
{
using Torque = Value<SiUnit<2, 1, -2, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Torque operator""_Nm(const long double magnitude)
{
    return units::si::Torque(magnitude);
}
