#pragma once

// Do not include this file anywhere except in Application.cpp. You are creating design issue!
// If you need access to a class contained in Application, just get yourself a reference to it
// via your classes' constructor.

#include "adc.h"
#include "i2c-drivers/rtos_accessor.hpp"
#include "i2c.h"
#include "spi.h"
#include "tim.h"

#include "LED/LightController.hpp"
#include "analog_to_digital/AnalogDigital.hpp"
#include "hall_encoder/HallEncoder.hpp"
#include "motor_control/MotorController.hpp"
#include "settings/Settings.hpp"
#include "state_machine/StateMachine.hpp"
#include "tactile_switches/TactileSwitches.hpp"

/// The entry point of users C++ firmware. This comes after CubeHAL and FreeRTOS initialization.
/// All needed classes and objects have the root here.
class Application
{
public:
    Application();
    [[noreturn]] void run();

    static Application &getApplicationInstance();

    static constexpr auto EepromBus = &hi2c1;
    i2c::RtosAccessor eepromBusAccessor{EepromBus}; // also for hall encoder
    Eeprom24LC64 eeprom{eepromBusAccessor, 0b000};

    static constexpr auto TmcUartPeripherie = &huart2;
    UartAccessor uartAccessorTmc{TmcUartPeripherie};

    // private:
    firmwareSettings::Container settingsContainer{};
    firmwareSettings::IO settingsIo{eeprom, settingsContainer};
    Settings settings{settingsIo};

    uint32_t overheatedCounter = 0;
    uint32_t warningTempCounter = 0;

    static constexpr auto AdcPeripherie = &hadc1;
    AnalogDigital analogDigital{AdcPeripherie};
    HallEncoder hallEncoder{settingsContainer, eepromBusAccessor};

    TactileSwitches tactileSwitches;
    MotorController motorController{settingsContainer, analogDigital, hallEncoder, uartAccessorTmc};
    StateMachine stateMachine{tactileSwitches, motorController};

    static constexpr util::PwmOutput8Bit RedChannel{&htim2, TIM_CHANNEL_1};
    static constexpr util::PwmOutput8Bit GreenChannel{&htim3, TIM_CHANNEL_1};
    util::pwm_led::DualLed<uint8_t> statusLed{RedChannel, GreenChannel};
    static constexpr auto LedSpiPeripherie = &hspi1;

    LightController lightController{LedSpiPeripherie, statusLed, settingsContainer, stateMachine,
                                    motorController};

private:
    static inline Application *instance{nullptr};
};