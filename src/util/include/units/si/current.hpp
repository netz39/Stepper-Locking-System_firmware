#pragma once

#include "units/scale.hpp"
#include "units/si.hpp"
#include "units/si_prefixes.hpp"

namespace units::si
{
using Current = Value<SiUnit<0, 0, 0, 1, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto mA = Scale<Current::Unit>{prefixes::inverse::milli};
constexpr auto uA = Scale<Current::Unit>{prefixes::inverse::micro};
} // namespace units::si::scale

constexpr units::si::Current operator""_A(const long double magnitude)
{
    return units::si::Current(magnitude);
}

constexpr units::si::Current operator""_A(const unsigned long long int magnitude)
{
    return units::si::Current(magnitude);
}

constexpr units::si::Current operator"" _mA(const long double magnitude)
{
    return units::si::Current(magnitude / units::si::prefixes::inverse::milli);
}

constexpr units::si::Current operator"" _mA(const unsigned long long int magnitude)
{
    return units::si::Current(magnitude / units::si::prefixes::inverse::milli);
}
