#include "wrappers/Mutex.hpp"
#include <core/SafeAssert.h>

namespace util::wrappers
{
Mutex::Mutex() : m_mutex(xSemaphoreCreateMutex())
{
    SafeAssert(m_mutex != nullptr);
}

void Mutex::lock()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
}
bool Mutex::lockWithTimeout(TickType_t timeToWait)
{
    return xSemaphoreTake(m_mutex, timeToWait) == pdPASS;
}
void Mutex::unlock()
{
    xSemaphoreGive(m_mutex);
}
}
