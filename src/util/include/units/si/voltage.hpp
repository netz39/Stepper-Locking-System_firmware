#pragma once

#include "units/scale.hpp"
#include "units/si.hpp"
#include "units/si_prefixes.hpp"

namespace units::si
{
using Voltage = Value<SiUnit<2, 1, -3, -1, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto mV = Scale<Voltage::Unit>{prefixes::inverse::milli};
constexpr auto uV = Scale<Voltage::Unit>{prefixes::inverse::micro};
} // namespace units::si::scale

constexpr units::si::Voltage operator""_V(const long double magnitude)
{
    return units::si::Voltage(magnitude);
}

constexpr units::si::Voltage operator""_V(const unsigned long long int magnitude)
{
    return units::si::Voltage(magnitude);
}

constexpr units::si::Voltage operator"" _mV(const long double magnitude)
{
    return units::si::Voltage(magnitude / units::si::prefixes::inverse::milli);
}

constexpr units::si::Voltage operator"" _mV(const unsigned long long int magnitude)
{
    return units::si::Voltage(magnitude / units::si::prefixes::inverse::milli);
}
