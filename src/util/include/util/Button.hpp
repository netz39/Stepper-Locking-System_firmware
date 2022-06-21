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

    explicit Button(util::Gpio buttonGpio, bool isInverted = false, Callback callback = nullptr)
        : buttonGpio{buttonGpio}, isInverted{isInverted}, callback{callback} {};

    Button(util::Gpio buttonGpio, const units::si::Time longPressTime, bool isInverted = false,
           Callback callback = nullptr)
        : buttonGpio{buttonGpio}, LongPressTime{longPressTime},
          isInverted{isInverted}, callback{callback} {};

    void update(units::si::Time timePassed);

    void setCallback(Callback newCallback)
    {
        callback = newCallback;
    }

    /// returns true if button state is long pressed at the moment
    [[nodiscard]] bool isLongPressing() const;

private:
    void loadTimer();
    void updateTimer(units::si::Time timePassed);
    [[nodiscard]] units::si::Time getPassedTime() const;

    static constexpr units::si::Time TimerReloadValue = 0.0_s;
    static constexpr units::si::Time DebounceTime = 20.0_ms;

    util::Gpio buttonGpio;
    const units::si::Time LongPressTime = 500.0_ms;
    bool isInverted = false;
    Callback callback;

    enum class InternalState
    {
        Idle,
        Pressed,
        LongPress
    };

    InternalState internalState = InternalState::Idle;
    units::si::Time pressTimer = TimerReloadValue;

    void buttonCallback(Action action);
};
} // namespace util