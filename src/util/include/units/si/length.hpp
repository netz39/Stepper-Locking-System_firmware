#pragma once

#include "units/si.hpp"

namespace units::si
{
using Length = Value<SiUnit<1, 0, 0, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Length operator"" _m(const long double magnitude)
{
    return units::si::Length(magnitude);
}
