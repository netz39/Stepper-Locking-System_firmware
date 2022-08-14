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

    BinarySemaphore(const BinarySemaphore & other) = delete;
    BinarySemaphore(BinarySemaphore && other) noexcept;
    BinarySemaphore &operator=(const BinarySemaphore & other) = delete;
    BinarySemaphore &operator= (BinarySemaphore && other) noexcept;

    BaseType_t take(TickType_t blocktime);
    BaseType_t giveFromISR(BaseType_t* pxHigherPriorityTaskWoken);
    BaseType_t give();
private:
    SemaphoreHandle_t handle{nullptr};
};

} // namespace drive_controller::wrapper