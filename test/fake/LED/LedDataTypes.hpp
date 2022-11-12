#pragma once

#include "LED/BgrColor.hpp"

static constexpr auto NumberOfRings = 2;
static constexpr auto NumberOfLedsInRing = 32;
static constexpr auto NumberOfLedsInCrossbar = 10;
static constexpr auto NumberOfLedsPerRing = NumberOfLedsInRing + NumberOfLedsInCrossbar;

class LedSegmentArray
{
public:
    BgrColor &operator[](int index)
    {
        assert(index >= 0);
        assert(index < NumberOfLedsPerRing);
        return dataArray[index];
    }

    BgrColor operator[](int index) const
    {
        assert(index >= 0);
        assert(index < NumberOfLedsPerRing);
        return dataArray[index];
    }

    constexpr size_t size() const
    {
        return dataArray.size();
    }

    constexpr BgrColor *data() noexcept
    {
        return dataArray.data();
    }

private:
    std::array<BgrColor, NumberOfLedsPerRing> dataArray{};
};