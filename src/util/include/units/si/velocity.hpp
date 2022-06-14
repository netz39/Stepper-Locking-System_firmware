#pragma once

#include "units/si.hpp"

namespace
{
constexpr auto KilometresPerHourFactor = 3.6f;
constexpr auto MilesPerHourFactor = 2.237f;
} // namespace

namespace units::si
{
using Velocity = Value<SiUnit<1, 0, -1, 0, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto km_h = Scale<Velocity::Unit>{KilometresPerHourFactor};
constexpr auto mi_h = Scale<Velocity::Unit>{MilesPerHourFactor};
} // namespace units::si::scale

constexpr units::si::Velocity operator"" _m_s(const long double magnitude)
{
    return units::si::Velocity(magnitude);
}

constexpr units::si::Velocity operator"" _km_h(const long double magnitude)
{
    return units::si::Velocity(magnitude / KilometresPerHourFactor);
}

constexpr units::si::Velocity operator"" _mi_h(const long double magnitude)
{
    return units::si::Velocity(magnitude / MilesPerHourFactor);
}
