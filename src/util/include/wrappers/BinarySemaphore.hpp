#pragma once
#include <FreeRTOS.h>
#include <semphr.h>

namespace util::wrappers
{

class BinarySemaphore
{
public:
    BinarySemaphore();
    ~BinarySemaphore();

    BinarySemaphore(const BinarySemaphore &) = delete;
    BinarySemaphore(BinarySemaphore &&) = delete;
    BinarySemaphore &operator=(const BinarySemaphore &) = delete;
    BinarySemaphore &operator= (BinarySemaphore &&) = delete;

    BaseType_t take(TickType_t blocktime);
    BaseType_t giveFromISR(BaseType_t* pxHigherPriorityTaskWoken);
    BaseType_t give();
private:
    SemaphoreHandle_t _handle{nullptr};
};

} // namespace drive_controller::wrapper