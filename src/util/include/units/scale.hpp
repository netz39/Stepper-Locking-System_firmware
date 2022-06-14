#pragma once

#include "units/si_unit.hpp"
#include <cstdint>

namespace units
{
template <typename SiUnit>
class Scale
{
public:
    constexpr Scale() : Scale(1.0f)
    {
    }

    constexpr explicit Scale(float factor) : factor{factor}
    {
    }

    constexpr float getScalingFactor() const
    {
        return factor;
    }

private:
    const float factor;
};
} // namespace units
