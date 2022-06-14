#pragma once

#include "units/si.hpp"
#include "units/si/time.hpp"

namespace units::si
{
using Energy = Value<SiUnit<2, 1, -2, 0, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto Wh = Scale<Energy::Unit>{1 / static_cast<float>(SecondsPerHour)};
}

constexpr units::si::Energy operator""_Ws(const long double magnitude)
{
    return units::si::Energy(magnitude);
}

constexpr units::si::Energy operator""_Wh(const long double magnitude)
{
    return units::si::Energy(static_cast<float>(SecondsPerHour) * magnitude);
}
