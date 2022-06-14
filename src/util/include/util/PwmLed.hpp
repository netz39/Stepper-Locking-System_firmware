#pragma once

#include "LedBase.hpp"
#include "util/PwmOutput.hpp"
#include <limits>

namespace util::pwm_led
{

//--------------------------------------------------------------------------------------------------
template <typename TimerResolution>
class SingleLed : public LedBase
{

public:
    explicit SingleLed(PwmOutput<TimerResolution> pwmOutput) : pwmOutput(pwmOutput){};

    void turnOnInherited() override
    {
        pwmOutput.setMaximumPwm();
    }

    void turnOffInherited() override
    {
        pwmOutput.setPwmValue(0);
    }

private:
    PwmOutput<TimerResolution> pwmOutput;
};

//--------------------------------------------------------------------------------------------------
enum class DualLedColor
{
    Red,
    Green,
    Yellow,
    Orange,
    DarkGreen,
    DarkRed
};

//--------------------------------------------------------------------------------------------------
template <typename TimerResolution>
class DualLed : public MultiColorLedBase<DualLedColor>
{
public:
    DualLed(PwmOutput<TimerResolution> ledRedPwmOutput,
            PwmOutput<TimerResolution> ledGreenPwmOutput)
        : ledRedPwmOutput(ledRedPwmOutput), ledGreenPwmOutput(ledGreenPwmOutput){};

private:
    void update() override
    {
        if (isOn)
        {
            switch (currentColor)
            {
            case DualLedColor::Red:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setPwmValue(0);
                break;

            case DualLedColor::Yellow:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setMaximumPwm();
                break;

            case DualLedColor::Orange:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 4);
                break;

            case DualLedColor::Green:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setMaximumPwm();
                break;

            case DualLedColor::DarkGreen:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 8);
                break;

            case DualLedColor::DarkRed:
                ledRedPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 8);
                ledGreenPwmOutput.setPwmValue(0);
                break;

            default:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setPwmValue(0);
                break;
            }
        }
        else
        {
            ledRedPwmOutput.setPwmValue(0);
            ledGreenPwmOutput.setPwmValue(0);
        }
    }

    PwmOutput<TimerResolution> ledRedPwmOutput;
    PwmOutput<TimerResolution> ledGreenPwmOutput;
};

//--------------------------------------------------------------------------------------------------
enum class TripleLedColor
{
    Red,
    Green,
    Blue,
    Yellow,
    Orange,
    Purple,
    Turquoise
};

//--------------------------------------------------------------------------------------------------
template <typename TimerResolution>
class TripleLed : public MultiColorLedBase<TripleLedColor>
{
public:
    TripleLed(PwmOutput<TimerResolution> ledRedPwmOutput,
              PwmOutput<TimerResolution> ledGreenPwmOutput,
              PwmOutput<TimerResolution> ledBluePwmOutput)
        : ledRedPwmOutput{ledRedPwmOutput}, ledGreenPwmOutput{ledGreenPwmOutput},
          ledBluePwmOutput{ledBluePwmOutput} {};

private:
    void update() override
    {
        if (isOn)
        {
            switch (currentColor)
            {
            case TripleLedColor::Red:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setPwmValue(0);
                ledBluePwmOutput.setPwmValue(0);
                break;

            case TripleLedColor::Yellow:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 3);
                ledBluePwmOutput.setPwmValue(0);
                break;

            case TripleLedColor::Orange:
                ledRedPwmOutput.setMaximumPwm();
                ledGreenPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 8);
                ledBluePwmOutput.setPwmValue(0);
                break;

            case TripleLedColor::Green:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setMaximumPwm();
                ledBluePwmOutput.setPwmValue(0);
                break;

            case TripleLedColor::Blue:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setPwmValue(0);
                ledBluePwmOutput.setMaximumPwm();
                break;

            case TripleLedColor::Turquoise:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setMaximumPwm();
                ledBluePwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 2);
                break;

            case TripleLedColor::Purple:
                ledRedPwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max());
                ledGreenPwmOutput.setPwmValue(0);
                ledBluePwmOutput.setPwmValue(std::numeric_limits<TimerResolution>::max() / 2);
                break;

            default:
                ledRedPwmOutput.setPwmValue(0);
                ledGreenPwmOutput.setPwmValue(0);
                ledBluePwmOutput.setPwmValue(0);
                break;
            }
        }
        else
        {
            ledRedPwmOutput.setPwmValue(0);
            ledGreenPwmOutput.setPwmValue(0);
            ledBluePwmOutput.setPwmValue(0);
        }
    }

    PwmOutput<TimerResolution> ledRedPwmOutput;
    PwmOutput<TimerResolution> ledGreenPwmOutput;
    PwmOutput<TimerResolution> ledBluePwmOutput;
};

} // namespace util::pwm_led