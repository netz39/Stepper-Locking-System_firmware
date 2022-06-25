#pragma once

namespace units::si::prefixes
{
constexpr auto giga = 1e+9f;
constexpr auto mega = 1e+6f;
constexpr auto kilo = 1e+3f;
constexpr auto deci = 1e-1f;
constexpr auto centi = 1e-2f;
constexpr auto milli = 1e-3f;
constexpr auto micro = 1e-6f;
constexpr auto nano = 1e-9f;
constexpr auto pico = 1e-12f;
} // namespace units::si::prefixes

namespace units::si::prefixes::inverse
{
constexpr auto giga = 1e-9f;
constexpr auto mega = 1e-6f;
constexpr auto kilo = 1e-3f;
constexpr auto deci = 1e+1f;
constexpr auto centi = 1e+2f;
constexpr auto milli = 1e+3f;
constexpr auto micro = 1e+6f;
constexpr auto nano = 1e+9f;
constexpr auto pico = 1e+12f;
} // namespace units::si::prefixes::inverse
