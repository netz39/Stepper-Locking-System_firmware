#pragma once

#include "gcem.hpp"
#include <array>
#include <cmath>
#include <cstdint>

constexpr auto PwmResolutionInBit = 8;

constexpr auto StartOffset = 1;
constexpr auto GammaFactor = 2.2;
constexpr auto MaximumIn = (1 << PwmResolutionInBit) - 1;
constexpr auto MaximumOut = (1 << PwmResolutionInBit) - 1;

using GammaTable = std::array<uint8_t, MaximumIn + 1>;

constexpr GammaTable createGammaTable()
{
    GammaTable gammaTable{};

    for (auto i = 1; i <= MaximumIn; i++)
    {
        const auto Logarithm =
            gcem::pow(static_cast<float>(i) / static_cast<float>(MaximumIn), GammaFactor);

        gammaTable[i] = StartOffset + gcem::round(Logarithm * (MaximumOut - StartOffset));
    }

    return gammaTable;
}

constexpr GammaTable GammaCorrectionLUT = createGammaTable();