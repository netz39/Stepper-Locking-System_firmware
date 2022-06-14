#pragma once

#include "helpers/freertos.hpp"

namespace util
{
class LedBase
{
public:
    virtual ~LedBase() = default;

    /// Turns on LED and resets it's blinking/flash state.
    void turnOn()
    {
        ledState = LedState::Normal;
        isOn = true;
    }

    /// Turns off LED and resets it's blinking/flash state.
    void turnOff()
    {
        ledState = LedState::Normal;
        isOn = false;
    }

    /// Enables blinking at given frequency.
    /// To turn off blinking simply use turnOn()/turnOff() functions.
    /// \param frequency Blink frequency - 50% of period time will be on/off
    void setBlinking(units::si::Frequency frequency)
    {
        ledState = LedState::Blinking;
        this->blinkFrequency = frequency;
    }

    /// Enables flashing.
    /// It fires two flashs every second.
    /// To turn off flashing simply use turnOn()/turnOff() functions.
    void setFlashing()
    {
        ledState = LedState::Flashing;
    }

    /// Updates the state of LED for blinking/flashing purposes.
    /// Should be called periodically.
    /// \param currentTicks the current system/task tick counter
    void updateState(uint32_t currentTicks)
    {
        switch (ledState)
        {
        case LedState::Blinking:
        {
            isOn = currentTicks % toOsTicks(blinkFrequency) / (toOsTicks(blinkFrequency) / 2);
        }
        break;

        case LedState::Flashing:
        {
            auto state = pdMS_TO_TICKS(currentTicks) % pdMS_TO_TICKS(1000) / pdMS_TO_TICKS(100);
            isOn = (state == 0 || state == 2);
        }
        break;

        case LedState::Normal:
        default:
            break;
        }

        update();
    }

protected:
    enum class LedState
    {
        Normal,
        Blinking,
        Flashing
    };

    LedState ledState = LedState::Normal;
    units::si::Frequency blinkFrequency = 1.0_Hz;

    /// Each derived class should implement the on/off process
    virtual void update() = 0;

    /// Normally used by inherited update functions to get the on/off state of LED object
    bool isOn = false;
};

//--------------------------------------------------------------------------------------------------
template <typename LedColor>
class MultiColorLedBase : public LedBase
{
public:
    /// Set the color of LED and turning it on automatically.
    /// States like blinking/flashing will be resetted.
    /// \param ledColor  color at which LED should lights up
    void setColor(LedColor ledColor)
    {
        ledState = LedState::Normal;
        currentColor = ledColor;
        isOn = true;
    }

    /// Enables blinking with given color at given frequency.
    /// To turn off blinking simply use turnOn()/turnOff() functions.
    /// \param ledColor  color at which LED should blink
    /// \param frequency Blink frequency - 50% of period time will be on/off
    void setColorBlinking(LedColor ledColor, units::si::Frequency frequency)
    {
        currentColor = ledColor;
        setBlinking(frequency);
    }

    /// Enables flashing with given color.
    /// To turn off flashing simply use turnOn()/turnOff() functions.
    /// \param ledColor  color at which LED should flashing
    void setColorFlashing(LedColor ledColor)
    {
        currentColor = ledColor;
        setFlashing();
    }

protected:
    /// Each derived class should implement it own enum class for colors and pass them as template
    /// parameter.
    LedColor currentColor{};
};

} // namespace util
