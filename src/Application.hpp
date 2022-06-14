#pragma once

#include "LED/LightController.hpp"
#include "tactile_switches/TactileSwitches.hpp"

class Application
{
public:
    Application();
    void run();

    static Application &getApplicationInstance();
    static void ledSpiCallback(SPI_HandleTypeDef *hspi);

private:
    LightController lightController;
    TactileSwitches tactileSwitches;
};