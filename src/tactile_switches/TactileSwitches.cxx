#include "TactileSwitches.hpp"

[[noreturn]] void TactileSwitches::taskMain()
{
    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        static constexpr auto ButtonSamplingInterval = 10.0_ms;
        openButton.update(ButtonSamplingInterval);
        closeButton.update(ButtonSamplingInterval);
        doorSwitch.update(ButtonSamplingInterval);
        lockSwitch.update(ButtonSamplingInterval);

        forceOpen.update(ButtonSamplingInterval);
        forceClose.update(ButtonSamplingInterval);
        vTaskDelayUntil(&lastWakeTime, toOsTicks(ButtonSamplingInterval));
    }
}