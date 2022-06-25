#pragma once

#include "units/scale.hpp"
#include "units/si.hpp"
#include "units/si_prefixes.hpp"

namespace units::si
{
using Resistance = Value<SiUnit<2, 1, -3, -2, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto MOhm = Scale<Resistance::Unit>{prefixes::inverse::mega};
constexpr auto kOhm = Scale<Resistance::Unit>{prefixes::inverse::kilo};
constexpr auto mOhm = Scale<Resistance::Unit>{prefixes::inverse::milli};
constexpr auto uOhm = Scale<Resistance::Unit>{prefixes::inverse::micro};
} // namespace units::si::scale

constexpr units::si::Resistance operator""_Ohm(const long double magnitude)
{
    return units::si::Resistance(magnitude);
}

constexpr units::si::Resistance operator""_Ohm(const unsigned long long int magnitude)
{
    return units::si::Resistance(magnitude);
}

constexpr units::si::Resistance operator"" _MOhm(const long double magnitude)
{
    return units::si::Resistance(magnitude * units::si::prefixes::mega);
}

constexpr units::si::Resistance operator"" _MOhm(const unsigned long long int magnitude)
{
    return units::si::Resistance(magnitude * units::si::prefixes::mega);
}

constexpr units::si::Resistance operator"" _kOhm(const long double magnitude)
{
    return units::si::Resistance(magnitude * units::si::prefixes::kilo);
}

constexpr units::si::Resistance operator"" _kOhm(const unsigned long long int magnitude)
{
    return units::si::Resistance(magnitude * units::si::prefixes::kilo);
}

constexpr units::si::Resistance operator"" _mOhm(const long double magnitude)
{
    return units::si::Resistance(magnitude / units::si::prefixes::inverse::milli);
}

constexpr units::si::Resistance operator"" _mOhm(const unsigned long long int magnitude)
{
    return units::si::Resistance(magnitude / units::si::prefixes::inverse::milli);
}