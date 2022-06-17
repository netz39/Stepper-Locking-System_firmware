#pragma once

#include "LED/LightController.hpp"
#include "analog_to_digital/AnalogDigital.hpp"
#include "tactile_switches/TactileSwitches.hpp"

class Application
{
public:
    Application();
    void run();

    static Application &getApplicationInstance();
    static void adcConversionCompleteCallback(ADC_HandleTypeDef *);
    static void ledSpiCallback(SPI_HandleTypeDef *hspi);

private:
    AnalogDigital analogDigital;
    LightController lightController;
    TactileSwitches tactileSwitches;
};