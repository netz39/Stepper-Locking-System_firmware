#pragma once
#include "wrappers/IMutex.hpp"
#include <FreeRTOS.h>
#include <semphr.h>

namespace util::wrappers
{
/**
 * Wrapper for FreeRTOS non-recursive mutex
 */
class RecursiveMutex : public IMutex
{
public:
    RecursiveMutex();
    ~RecursiveMutex() override;

    RecursiveMutex(const RecursiveMutex &) = delete;
    RecursiveMutex(RecursiveMutex &&other) noexcept;
    RecursiveMutex &operator=(const RecursiveMutex &) = delete;
    RecursiveMutex &operator=(RecursiveMutex &&other) noexcept;

    void lock() override;
    bool lockWithTimeout(TickType_t timeToWait) override;
    void unlock() override;

private:
    SemaphoreHandle_t m_mutex{nullptr};
};
} // namespace util::wrappers