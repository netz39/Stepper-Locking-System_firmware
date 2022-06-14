#pragma once

#include "gpio.hpp"
#include <units/si/time.hpp>

#include <functional>

namespace util
{
class Button
{
public:
    enum class State
    {
        NotPressed = 0,
        Pressed = 1
    };

    enum class Action
    {
        ShortPress,
        LongPress,
        StopLongPress
    };

    using Callback = std::function<void(Action action)>;

    Button(util::Gpio buttonGpio, Callback callback, bool isInverted = false)
        : buttonGpio{buttonGpio}, ButtonCallback{callback}, isInverted{isInverted} {};

    Button(util::Gpio buttonGpio, Callback callback, const units::si::Time longPressTime,
           bool isInverted = false)
        : buttonGpio{buttonGpio}, ButtonCallback{callback}, LongPressTime{longPressTime},
          isInverted{isInverted} {};

    void update(units::si::Time timePassed);

    [[nodiscard]] bool getState() const;

private:
    void loadTimer();
    void updateTimer(units::si::Time timePassed);
    [[nodiscard]] units::si::Time getPassedTime() const;

    static constexpr units::si::Time TimerReloadValue = 0.0_s;
    static constexpr units::si::Time DebounceTime = 50.0_ms;

    util::Gpio buttonGpio;
    const Callback ButtonCallback;
    const units::si::Time LongPressTime = 500.0_ms;
    bool isInverted = false;

    enum class InternalState
    {
        Idle,
        Pressed,
        LongPress
    };

    InternalState internalState = InternalState::Idle;
    units::si::Time pressTimer = TimerReloadValue;
};
} // namespace util