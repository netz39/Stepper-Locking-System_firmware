#pragma once
#include <FreeRTOS.h>

namespace util::wrappers
{
class IMutex
{
public:
    virtual ~IMutex() = default;

    /**
     * Lock the mutex, waits without timeout for acquisition.
     */
    virtual void lock() = 0;

    /**
     * Attempts to lock the mutex within given time.
     * @param timeToWait maximum FreeRTOS ticks to wait
     * @return false if acquisition was not possible in time. true on success
     */
    virtual bool lockWithTimeout(TickType_t timeToWait) = 0;

    /**
     * Unlocks the mutex.
     */
    virtual void unlock() = 0;
};
}