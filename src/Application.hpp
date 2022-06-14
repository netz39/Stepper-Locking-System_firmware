#pragma once

#include "LED/LightController.hpp"
#include "tactile_switches/TactileSwitches.hpp"

class Application
{
public:
    Application();
    void run();

    static Application &getApplicationInstance();

private:
    LightController lightController;
    TactileSwitches tactileSwitches;
};