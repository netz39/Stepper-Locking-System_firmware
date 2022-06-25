#include "wrappers/RecursiveMutex.hpp"
#include <core/SafeAssert.h>

namespace util::wrappers
{
RecursiveMutex::RecursiveMutex() : m_mutex(xSemaphoreCreateRecursiveMutex())
{
    SafeAssert(m_mutex != nullptr);
}

void RecursiveMutex::lock()
{
    xSemaphoreTakeRecursive(m_mutex, portMAX_DELAY);
}
bool RecursiveMutex::lockWithTimeout(TickType_t timeToWait)
{
    return xSemaphoreTakeRecursive(m_mutex, timeToWait) == pdPASS;
}
void RecursiveMutex::unlock()
{
    xSemaphoreGiveRecursive(m_mutex);
}
}
