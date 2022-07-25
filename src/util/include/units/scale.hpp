#pragma once

#include "units/si_unit.hpp"
#include <cstdint>

namespace units
{
template <typename SiUnit>
class Scale
{
public:
    constexpr Scale() = default;

    constexpr explicit Scale(float factor) : factor{factor}
    {
    }

    [[nodiscard]] constexpr float getScalingFactor() const
    {
        return factor;
    }

private:
    const float factor{};
};
} // namespace units
