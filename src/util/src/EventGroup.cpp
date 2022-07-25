#include "wrappers/EventGroup.hpp"
#include <utility>

namespace util::wrappers
{

EventGroup::EventGroup() : handle(xEventGroupCreate())
{
}
EventGroup::~EventGroup()
{
    if (handle != nullptr)
    {
        vEventGroupDelete(handle);
    }
}
EventBits_t EventGroup::clearBits(const EventBits_t uxBitsToClear)
{
    return xEventGroupClearBits(handle, uxBitsToClear);
}
bool EventGroup::clearBitsFromISR(const EventBits_t uxBitsToClear)
{
    return xEventGroupClearBitsFromISR(handle, uxBitsToClear) == pdPASS;
}
EventBits_t EventGroup::getBits()
{
    return xEventGroupGetBits(handle);
}
EventBits_t EventGroup::getBitsFromISR()
{
    return xEventGroupGetBitsFromISR(handle);
}
EventBits_t EventGroup::setBits(const EventBits_t bits)
{
    return xEventGroupSetBits(handle, bits);
}
EventBits_t EventGroup::setBitsFromISR(const EventBits_t bits, BaseType_t *pxHigherPriorityTaskWoken)
{
    return xEventGroupSetBitsFromISR(handle, bits, pxHigherPriorityTaskWoken);
}
EventBits_t EventGroup::sync(const EventBits_t bitsToSet, const EventBits_t bitsToWaitFor, const TickType_t waitTime)
{
    return xEventGroupSync(handle, bitsToSet, bitsToWaitFor, waitTime);
}
EventBits_t EventGroup::waitBits(const EventBits_t bitsToWaitFor, const bool clearOnExit, const bool waitForAll,
                                 const TickType_t waitTime)
{
    return xEventGroupWaitBits(handle, bitsToWaitFor, clearOnExit ? pdTRUE : pdFALSE,
                               waitForAll ? pdTRUE : pdFALSE, waitTime);
}
EventGroup::EventGroup(EventGroup &&other) noexcept
{
        (*this) = std::forward<EventGroup>(other);
}
EventGroup &EventGroup::operator=(EventGroup &&other) noexcept
{
    handle = std::exchange(other.handle, nullptr);
    return *this;
}

} // namespace util::wrappers