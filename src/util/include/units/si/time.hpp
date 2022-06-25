#pragma once

#include "units/scale.hpp"
#include "units/si.hpp"
#include "units/si_prefixes.hpp"

namespace
{
constexpr auto MicrosecondsPerSecond = 1'000'000;
constexpr auto MillisecondsPerSecond = 1'000;
constexpr auto SecondsPerMinute = 60;
constexpr auto SecondsPerHour = 3600;
} // namespace

namespace units::si
{
using Time = Value<SiUnit<0, 0, 1, 0, 0, 0, 0>>;
} // namespace units::si

namespace units::si::scale
{
constexpr auto h = Scale<Time::Unit>{1 / static_cast<float>(SecondsPerHour)};
constexpr auto min = Scale<Time::Unit>{1 / static_cast<float>(SecondsPerMinute)};
constexpr auto ms = Scale<Time::Unit>{prefixes::inverse::milli};
constexpr auto us = Scale<Time::Unit>{prefixes::inverse::micro};
constexpr auto ns = Scale<Time::Unit>{prefixes::inverse::nano};
} // namespace units::si::scale

constexpr units::si::Time operator"" _us(const long double magnitude)
{
    return units::si::Time(magnitude / MicrosecondsPerSecond);
}

constexpr units::si::Time operator"" _ms(const long double magnitude)
{
    return units::si::Time(magnitude / MillisecondsPerSecond);
}

constexpr units::si::Time operator"" _s(const long double magnitude)
{
    return units::si::Time(magnitude);
}

constexpr units::si::Time operator"" _min(const long double magnitude)
{
    return units::si::Time(magnitude * SecondsPerMinute);
}

constexpr units::si::Time operator"" _h(const long double magnitude)
{
    return units::si::Time(magnitude * SecondsPerHour);
}
