#include "TactileSwitches.hpp"

#include "Application.hpp"

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
        vTaskDelayUntil(&lastWakeTime, toOsTicks(ButtonSamplingInterval));
    }
}