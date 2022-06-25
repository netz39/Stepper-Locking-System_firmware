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
    virtual ~Mutex() = default;

    virtual void lock() override;
    virtual bool lockWithTimeout(TickType_t timeToWait) override;
    virtual void unlock() override;
private:
    SemaphoreHandle_t m_mutex;
};
}