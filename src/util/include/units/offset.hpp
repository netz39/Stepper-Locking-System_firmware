#pragma once

#include "units/si_unit.hpp"
#include <cstdint>

namespace units
{
template <typename SiUnit>
class Offset
{
public:
    constexpr Offset() : Offset(0.0f)
    {
    }

    constexpr explicit Offset(float offset) : offset{offset}
    {
    }

    constexpr float getOffset() const
    {
        return offset;
    }

private:
    const float offset;
};
} // namespace units
