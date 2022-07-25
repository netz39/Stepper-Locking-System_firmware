#include "wrappers/Mutex.hpp"
#include <core/SafeAssert.h>
#include <utility>

namespace util::wrappers
{
Mutex::Mutex() : m_mutex(xSemaphoreCreateMutex())
{
    SafeAssert(m_mutex != nullptr);
}

Mutex::~Mutex()
{
    if (m_mutex != nullptr)
    {
        vSemaphoreDelete(m_mutex);
    }
}

void Mutex::lock()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
}

bool Mutex::lockWithTimeout(const TickType_t timeToWait)
{
    return xSemaphoreTake(m_mutex, timeToWait) == pdPASS;
}

void Mutex::unlock()
{
    xSemaphoreGive(m_mutex);
}

Mutex::Mutex(Mutex &&other) noexcept
{
    (*this) = std::move(other);
}

Mutex &Mutex::operator=(Mutex &&other) noexcept
{
    m_mutex = std::exchange(other.m_mutex, nullptr);
    return *this;
}

} // namespace util::wrappers
