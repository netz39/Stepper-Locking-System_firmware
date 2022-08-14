#include "wrappers/RecursiveMutex.hpp"
#include <core/SafeAssert.h>
#include <utility>

namespace util::wrappers
{
RecursiveMutex::RecursiveMutex() : m_mutex(xSemaphoreCreateRecursiveMutex())
{
    SafeAssert(m_mutex != nullptr);
}

RecursiveMutex::~RecursiveMutex()
{
    if (m_mutex != nullptr)
    {
        vSemaphoreDelete(m_mutex);
    }
}

void RecursiveMutex::lock()
{
    xSemaphoreTakeRecursive(m_mutex, portMAX_DELAY);
}

bool RecursiveMutex::lockWithTimeout(const TickType_t timeToWait)
{
    return xSemaphoreTakeRecursive(m_mutex, timeToWait) == pdPASS;
}

void RecursiveMutex::unlock()
{
    xSemaphoreGiveRecursive(m_mutex);
}

RecursiveMutex::RecursiveMutex(RecursiveMutex &&other) noexcept
{
    (*this) = std::move(other);
}

RecursiveMutex &RecursiveMutex::operator=(RecursiveMutex &&other) noexcept
{
    m_mutex = std::exchange(other.m_mutex, nullptr);
    return *this;
}

} // namespace util::wrappers
