#include "HallEncoder.hpp"
#include "sync.hpp"

#include "helpers/freertos.hpp"
#include "units/si/frequency.hpp"

constexpr auto TaskFrequency = 100.0_Hz;

//--------------------------------------------------------------------------------------------------
void HallEncoder::taskMain()
{
    // wait for EEPROM
    sync::waitForAll(sync::ConfigurationLoaded);

    configureHall();

    auto lastWakeTime = xTaskGetTickCount();
    while (true)
    {
        device.run(false);
        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));
    }
}

//--------------------------------------------------------------------------------------------------
void HallEncoder::configureHall()
{
    device.configureDevice(
        AS5600::AS5600::PowerMode::LPM1,      // LPM1 -> polling time 5 ms - current = ~3.4mA
        AS5600::AS5600::HysteresisMode::LSB1, // one LSB filter
        AS5600::AS5600::SlowFilterMode::x16,  // default value
        AS5600::AS5600::FastFilterThreshold::slowFilter, // default value
        true                                             // watchdog, power saving after one minute
    );
}