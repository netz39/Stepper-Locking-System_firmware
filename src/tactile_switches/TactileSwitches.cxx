#include "TactileSwitches.hpp"

#include "Application.hpp"

void TactileSwitches::openButtonCallback(util::Button::Action action)
{
}

//--------------------------------------------------------------------------------------------------
void TactileSwitches::closeButtonCallback(util::Button::Action action)
{
}

//--------------------------------------------------------------------------------------------------
void TactileSwitches::doorSwitchCallback(util::Button::Action action)
{
}

//--------------------------------------------------------------------------------------------------
void TactileSwitches::lockSwitchCallback(util::Button::Action action)
{
}

//--------------------------------------------------------------------------------------------------
void TactileSwitches::taskMain()
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