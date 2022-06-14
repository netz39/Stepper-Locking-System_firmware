#include "Port.h"
#include "main.h"

GPIO_TypeDef *pinToGPIOBase(int8_t pin)
{
    switch (pin)
    {
    case PA3:
    case PA5:
    case PA6:
        return GPIOA;
    default:
        teensyStepErrorHandler();
        break;
    }
}

uint16_t pinToGPIOPinNumber(int8_t pin)
{
    uint16_t pinNumber = 0;
    switch (pin)
    {
    case PA3:
        pinNumber = GPIO_PIN_3;
        break;
    case PA5:
        pinNumber = GPIO_PIN_5;
        break;
    case PA6:
        pinNumber = GPIO_PIN_6;
        break;
    default:
        teensyStepErrorHandler();
        break;
    }
    return pinNumber;
}

void digitalWrite(int8_t pin, uint8_t polarity)
{
    if (polarity == 1 || polarity == 0)
    {
        HAL_GPIO_WritePin(pinToGPIOBase(pin), pinToGPIOPinNumber(pin),
                          polarity == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
    else
    {
        teensyStepErrorHandler();
    }
}
void digitalToggle(int8_t pin)
{
    HAL_GPIO_TogglePin(pinToGPIOBase(pin), pinToGPIOPinNumber(pin));
}
void pinMode(int8_t pin, uint8_t polarity)
{
    // already set in the cubemx project, functions therefore does nothing.
}
void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t dwt_getCycles()
{
    // return (DWT->CYCCNT);
    return SysTick->VAL;
}

void teensyStepErrorHandler()
{
    while (true)
    {
        HAL_GPIO_TogglePin(LED_Red_GPIO_Port, LED_Red_Pin); // let red led flash

#ifdef DEBUG
        __asm("bkpt");
#endif
        HAL_Delay(500);
    }
}
void _Error_Handler(const char *, int)
{
    teensyStepErrorHandler();
}
