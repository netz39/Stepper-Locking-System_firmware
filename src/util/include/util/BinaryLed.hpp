#pragma once
#include "LedBase.hpp"
#include "util/gpio.hpp"

namespace util::binary_led
{
//--------------------------------------------------------------------------------------------------
class SingleLed : public LedBase
{
public:
    explicit SingleLed(Gpio gpio) : ledGpio{gpio} {};

private:
    void update()
    {
        ledGpio.write(isOn);
    }

    Gpio ledGpio;
};

enum class DualLedColor
{
    Red,
    Green,
    Yellow
};

//--------------------------------------------------------------------------------------------------

/// only for red and green LEDs in bundle
class DualLed : public MultiColorLedBase<DualLedColor>
{
public:
    DualLed(Gpio ledRedGpio, Gpio ledGreenGpio)
        : ledRedGpio{ledRedGpio}, ledGreenGpio{ledGreenGpio} {};

private:
    void update() override
    {
        if (isOn)
        {
            ledRedGpio.write((currentColor == DualLedColor::Red || currentColor == DualLedColor::Yellow));
            ledGreenGpio.write((currentColor == DualLedColor::Green || currentColor == DualLedColor::Yellow));
        }
        else
        {
            ledRedGpio.write(Gpio::Low);
            ledGreenGpio.write(Gpio::Low);
        }
    }

    Gpio ledRedGpio;
    Gpio ledGreenGpio;
};
} // namespace util::binary_led
