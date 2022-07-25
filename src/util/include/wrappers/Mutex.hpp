#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include "wrappers/IMutex.hpp"

namespace util::wrappers
{
/**
 * Wrapper for FreeRTOS non-recursive mutex
 */
class Mutex : public IMutex
{
public:
    Mutex();
    ~Mutex() override;

    Mutex(const Mutex &) = delete;
    Mutex(Mutex &&other) noexcept;
    Mutex &operator=(const Mutex &) = delete;
    Mutex &operator=(Mutex &&other) noexcept;

    void lock() override;
    bool lockWithTimeout(TickType_t timeToWait) override;
    void unlock() override;
private:
    SemaphoreHandle_t m_mutex{nullptr};
};
}