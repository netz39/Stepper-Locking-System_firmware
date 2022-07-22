#pragma once

#include "i2c-drivers/rtos_accessor.hpp"
#include "i2c.h"

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

    Temperature motorTemperature{};
    uint32_t overheatedCounter = 0;
    uint32_t warningTempCounter = 0;

    AnalogDigital analogDigital{motorTemperature};
    HallEncoder hallEncoder{settingsContainer, eepromBusAccessor};

    TactileSwitches tactileSwitches;
    MotorController motorController{settingsContainer, motorTemperature, hallEncoder,
                                    uartAccessorTmc};
    StateMachine stateMachine{tactileSwitches, motorController};

    LightController lightController{settingsContainer, stateMachine, motorController};

    static void adcConversionCompleteCallback(ADC_HandleTypeDef *);
    static void ledSpiCallback(SPI_HandleTypeDef *);
    static void i2cMasterCmpltCallback(I2C_HandleTypeDef *);
    static void i2cErrorCallback(I2C_HandleTypeDef *);
    static void uartTmcCmpltCallback(UART_HandleTypeDef *);
    static void uartTmcErrorCallback(UART_HandleTypeDef *);

private:
    static inline Application* instance{nullptr};
};