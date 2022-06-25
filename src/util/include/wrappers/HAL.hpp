#pragma once
#include <FreeRTOS.h>
#include <hal_header.h>

namespace util::wrappers
{

using TimeTick = uint32_t;
class HAL
{
public:
    HAL() = default;
    virtual ~HAL() = default;

    HAL(const HAL &) = delete;
    HAL(HAL &&) = delete;
    HAL &operator=(const HAL &) = delete;
    HAL &operator=(HAL &&) = delete;

    virtual uint32_t getTick() const
    {
        return HAL_GetTick();
    }
    static constexpr TimeTick TimeTicksPerSecond = 1000;
};

} // namespace wrapper