#pragma once

#include "BgrColor.hpp"
#include <array>

static constexpr auto NumberOfRings = 2;
static constexpr auto NumberOfLedsInRing = 32;
static constexpr auto NumberOfLedsInCrossbar = 10;
static constexpr auto NumberOfLedsPerRing = NumberOfLedsInRing + NumberOfLedsInCrossbar;

/// array which should be filled by user/animation class with data
using LedSegmentArray = std::array<BgrColor, NumberOfLedsPerRing>;