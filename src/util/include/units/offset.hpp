#pragma once

#include "units/si_unit.hpp"
#include <cstdint>

namespace units
{
template <typename SiUnit>
class Offset
{
public:
    constexpr Offset() = default;

    constexpr explicit Offset(float offset) : offset{offset}
    {
    }

    [[nodiscard]] constexpr float getOffset() const
    {
        return offset;
    }

private:
    const float offset{};
};
} // namespace units
