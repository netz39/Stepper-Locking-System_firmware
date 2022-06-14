#include "wrappers/BinarySemaphore.hpp"
#include <core/SafeAssert.h>

namespace util::wrappers
{

BinarySemaphore::BinarySemaphore() : _handle(xSemaphoreCreateBinary())
{
    SafeAssert(_handle != nullptr);
}

BinarySemaphore::~BinarySemaphore()
{
    if (_handle != nullptr)
    {
        vSemaphoreDelete(_handle);
    }
}

BaseType_t BinarySemaphore::take(TickType_t blocktime) 
{
    return xSemaphoreTake(_handle, blocktime);
}

BaseType_t BinarySemaphore::giveFromISR(BaseType_t* pxHigherPriorityTaskWoken)
{
    return xSemaphoreGiveFromISR(_handle, pxHigherPriorityTaskWoken);
}
BaseType_t BinarySemaphore::give()
{
    return xSemaphoreGive(_handle);
}

} // namespace drive_controller::wrapper