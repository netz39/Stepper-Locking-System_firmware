#include "FreeRTOS.h"
#include "helpers/freertos.hpp"
#include "task.h"

#include "LightController.hpp"

#include <algorithm>

void LightController::taskMain()
{
    RedChannel.startPwmTimer();
    GreenChannel.startPwmTimer();

    auto lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        statusLed.updateState(lastWakeTime);

        static constexpr auto TaskFrequency = 20.0_Hz;
        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));
    }
}