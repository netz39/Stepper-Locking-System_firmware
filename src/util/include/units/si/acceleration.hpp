#pragma once

#include "units/si.hpp"

namespace units::si
{
using Acceleration = Value<SiUnit<1, 0, -2, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Acceleration operator"" _m_s2(const long double magnitude)
{
    return units::si::Acceleration(magnitude);
}
