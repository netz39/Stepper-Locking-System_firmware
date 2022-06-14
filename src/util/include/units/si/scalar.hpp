#pragma once

#include "units/si.hpp"

namespace units::si
{
using Scalar = Value<SiUnit<0, 0, 0, 0, 0, 0, 0>>;
} // namespace units::si

constexpr units::si::Scalar operator""_(const long double magnitude)
{
    return units::si::Scalar(magnitude);
}

constexpr units::si::Scalar operator""_(const unsigned long long int magnitude)
{
    return units::si::Scalar(magnitude);
}
