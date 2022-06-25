#pragma once
#include <FreeRTOS.h>
#include <semphr.h>
#include "wrappers/IMutex.hpp"

namespace util::wrappers
{
/**
 * Wrapper for FreeRTOS non-recursive mutex
 */
class RecursiveMutex : public IMutex
{
public:
    RecursiveMutex();
    virtual ~RecursiveMutex() = default;

    virtual void lock() override;
    virtual bool lockWithTimeout(TickType_t timeToWait) override;
    virtual void unlock() override;
private:
    SemaphoreHandle_t m_mutex;
};
}