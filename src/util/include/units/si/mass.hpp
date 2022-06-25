#pragma once

#include "units/si.hpp"

namespace units::si
{
using Mass = Value<SiUnit<0, 1, 0, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Mass operator"" _kg(const long double magnitude)
{
    return units::si::Mass(magnitude);
}
