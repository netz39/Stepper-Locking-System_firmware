#pragma once

#include "units/si.hpp"

namespace units::si
{
using Temperature = Value<SiUnit<0, 0, 0, 0, 1, 0, 0>>;
} // namespace units::si

namespace units::si::offset
{
constexpr auto degC = Offset<Temperature::Unit>{-273.15f};
}

constexpr units::si::Temperature operator""_K(const long double magnitude)
{
    return units::si::Temperature(magnitude);
}

constexpr units::si::Temperature operator""_degC(const long double magnitude)
{
    return units::si::Temperature(magnitude - units::si::offset::degC.getOffset());
}