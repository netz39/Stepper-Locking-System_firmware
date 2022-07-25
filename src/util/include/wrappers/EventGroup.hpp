#pragma once
#include <FreeRTOS.h>
#include <core/SafeAssert.h>
#include <event_groups.h>

namespace util::wrappers
{
class EventGroup
{
public:
    EventGroup();
    ~EventGroup();

    EventGroup(const EventGroup & other) = delete;
    EventGroup(EventGroup && other) noexcept;
    EventGroup &operator=(const EventGroup & other) = delete;
    EventGroup &operator= (EventGroup && other) noexcept;

    EventBits_t clearBits(EventBits_t uxBitsToClear);
    bool clearBitsFromISR(EventBits_t uxBitsToClear);

    EventBits_t getBits();
    EventBits_t getBitsFromISR();

    EventBits_t setBits(EventBits_t bits);
    EventBits_t setBitsFromISR(EventBits_t bits, BaseType_t *pxHigherPriorityTaskWoken);

    EventBits_t sync(EventBits_t bitsToSet, EventBits_t bitsToWaitFor, TickType_t waitTime);

    EventBits_t waitBits(EventBits_t bitsToWaitFor, bool clearOnExit, bool waitForAll,
                         TickType_t waitTime);
private:
    EventGroupHandle_t handle{nullptr};
};

} // namespace util::wrappers