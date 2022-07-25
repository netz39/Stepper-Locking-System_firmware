# util namespace

## Gpio class
*ToDo*

## LED class

#### binary LED
A LED can be controlled by simply turning it on or off, without dimming.
This can be represented by `util::binary_led::SingleLed` with `util::Gpio` as parameter:

```cpp
#include "util/BinaryLed.hpp"

constexpr util::Gpio TestLedGpio(TEST_LED_GPIO_Port, TEST_LED_Pin);
util::binary_led::SingleLed testLed(TestLedGpio);
```

#### PWM LED

For a multicolor LED it makes more sense to control each LED by PWM to get a various color mixings.
For instance, an RGB LED can be represented by `util::pwm_led::TripleLed` with three `util::PwmOutput8Bit` (for 8-Bit PWM timer) as parameter:

```cpp
#include "util/PwmLed.hpp"

constexpr auto TimerHandle = &htim1;

constexpr util::PwmOutput8Bit RedChannel(TimerHandle, TIM_CHANNEL_1);
constexpr util::PwmOutput8Bit GreenChannel(TimerHandle, TIM_CHANNEL_2);
constexpr util::PwmOutput8Bit BlueChannel(TimerHandle, TIM_CHANNEL_3);

util::pwm_led::TripleLed rgbLed(RedChannel, GreenChannel, BlueChannel);
```

### How to use
You can apply modes like blinking, flashing, on/off state to your object and for multicolor LED you can change its color mix of course.

For single color LEDs (regardless of binary of PWM types), we have the following functions:
- `turnOn()`
- `turnOff()`
- `setBlinking(units::si::Freqency frequency)`
- `setFlashing()`

For multicolor LEDs, we have those additional functions:
- `setColor(LEDColor ledColor)`
- `setColorBlinking(LEDColor ledColor,units::si::Freqency frequency)`

**Make sure** that blinking/flashing works properly, the `updateState()` function for each LED object must be called periodically.

```cpp
constexpr auto TaskFrequency = 50.0_Hz;

extern "C" void taskFunction(void *)
{
    // turn on LED and let them blinking with 500ms on/off times
    testLed.setBlinking(1.0_Hz);

    // set LED color to Orange and turn that on
    rgbLed.setColor(util::pwm_led::TripleLedColor::Orange);

    auto lastWakeTime = xTaskGetTickCount();
    while(true)
    {
        testLed.updateState(lastWakeTime);
        rgbLed.updateState(lastWakeTime);

        vTaskDelayUntil(&lastWakeTime, toOsTicks(TaskFrequency));
    }
}
```

## PwmOutput class
*ToDo*

## Button class
*ToDo*

# unit namespace
*ToDo*

# FreeRTOS wrappers

## Task
*ToDo*