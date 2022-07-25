#include "wrappers/BinarySemaphore.hpp"
#include <core/SafeAssert.h>
#include <utility>

namespace util::wrappers
{

BinarySemaphore::BinarySemaphore() : handle(xSemaphoreCreateBinary())
{
    SafeAssert(handle != nullptr);
}

BinarySemaphore::~BinarySemaphore()
{
    if (handle != nullptr)
    {
        vSemaphoreDelete(handle);
    }
}

BaseType_t BinarySemaphore::take(const TickType_t blocktime)
{
    return xSemaphoreTake(handle, blocktime);
}

BaseType_t BinarySemaphore::giveFromISR(BaseType_t* pxHigherPriorityTaskWoken)
{
    return xSemaphoreGiveFromISR(handle, pxHigherPriorityTaskWoken);
}

BaseType_t BinarySemaphore::give()
{
    return xSemaphoreGive(handle);
}

BinarySemaphore::BinarySemaphore(BinarySemaphore &&other) noexcept
{
    (*this) = std::forward<BinarySemaphore>(other);
}

BinarySemaphore &BinarySemaphore::operator=(BinarySemaphore &&other) noexcept
{
    handle = std::exchange(other.handle, nullptr);
    return *this;
}

} // namespace drive_controller::wrapper