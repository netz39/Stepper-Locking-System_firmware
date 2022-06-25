#pragma once

#include "units/si.hpp"

namespace units::si
{
using Power = Value<SiUnit<2, 1, -3, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Power operator""_W(const long double magnitude)
{
    return units::si::Power(magnitude);
}
