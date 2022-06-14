#pragma once

#include "units/si.hpp"

namespace units::si
{
using Inertia = Value<SiUnit<2, 1, 0, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Inertia operator"" _m2kg(const long double magnitude)
{
    return units::si::Inertia(magnitude);
}
