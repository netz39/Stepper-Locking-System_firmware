#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include <string_view>
#include <units/si/time.hpp>

namespace util::wrappers
{

enum class NotifyAction
{
    NoAction,
    SetBits,
    Increment,
    SetValueWithOverwrite,
    SetValueWithoutOverwrite
};

/// Abstraction to make running embedded code easier to run on pc
class IFreeRTOSTask
{
public:
    IFreeRTOSTask(const std::string_view name, uint16_t stackDepth, uint32_t priority)
        : taskName(name), taskStackDepth(stackDepth), taskPriority(priority){};
    virtual ~IFreeRTOSTask() = default;

    virtual int32_t notifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit,
                               uint32_t *pulNotificationValue, uint32_t xTicksToWait) = 0;

    virtual int32_t notify(uint32_t ulValue, NotifyAction eAction) = 0;

    virtual int32_t notifyFromISR(uint32_t ulValue, NotifyAction eAction,
                                  int32_t *pxHigherPriorityTaskWoken) = 0;

    virtual void notifyGive() = 0;
    virtual void notifyTake(uint32_t waittime) = 0;
    virtual void delay(units::si::Time time) = 0;

protected:
    std::string_view taskName;
    const uint16_t taskStackDepth;
    const uint32_t taskPriority;
};

} // namespace util::wrappers