#pragma once

#include "i2c-drivers/rtos_accessor.hpp"
#include "i2c.h"

#include "LED/LightController.hpp"
#include "analog_to_digital/AnalogDigital.hpp"
#include "motor_control/MotorController.hpp"
#include "settings/Settings.hpp"
#include "state_machine/StateMachine.hpp"
#include "tactile_switches/TactileSwitches.hpp"

class Application
{
public:
    Application();
    void run();

    static Application &getApplicationInstance();

    static constexpr auto EepromBus = &hi2c1;
    i2c::RtosAccessor eepromBusAccessor{EepromBus};
    Eeprom24LC64 eeprom{eepromBusAccessor, 0b000};

    // private:
    firmwareSettings::Container settingsContainer;
    firmwareSettings::IO settingsIo{eeprom, settingsContainer};
    Settings settings{settingsIo};

    Temperature motorTemperature{};
    uint32_t overheatedCounter = 0;
    uint32_t warningTempCounter = 0;

    AnalogDigital analogDigital{motorTemperature};

    TactileSwitches tactileSwitches;
    MotorController motorController{settingsContainer, motorTemperature};
    StateMachine stateMachine{tactileSwitches, motorController};

    LightController lightController{settingsContainer,stateMachine, motorController};

    static void adcConversionCompleteCallback(ADC_HandleTypeDef *);
    static void ledSpiCallback(SPI_HandleTypeDef *);
    static void i2cMasterTxCallback(I2C_HandleTypeDef *);
    static void i2cMasterRxCallback(I2C_HandleTypeDef *);
    static void i2cErrorCallback(I2C_HandleTypeDef *);
};