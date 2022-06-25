#pragma once

#include "BgrColor.hpp"
#include "GammaCorrection.hpp"

struct LedSpiData
{
    uint8_t Start = 0xFF;
    BgrColor color;

    void assignGammaCorrectedColor(BgrColor newColor)
    {
        color.blue = GammaCorrectionLUT[newColor.blue];
        color.green = GammaCorrectionLUT[newColor.green];
        color.red = GammaCorrectionLUT[newColor.red];
    }
};

static constexpr auto NumberOfRings = 2;
static constexpr auto NumberOfLedsInRing = 32;
static constexpr auto NumberOfLedsInCrossbar = 10;
static constexpr auto NumberOfLedsPerRing = NumberOfLedsInRing + NumberOfLedsInCrossbar;

/// array which should be filled by user/animation class with data
using LedSegmentArray = std::array<BgrColor, NumberOfLedsPerRing>;

/// array, in which its content is directly sended over SPI - contains control data
using LedSpiDataArray = std::array<LedSpiData, NumberOfLedsPerRing>;