#include "Port.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

GPIO_TypeDef *pinToGPIOBase(int8_t pin)
{
    switch (pin)
    {
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
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t dwt_getCycles()
{
    // return (DWT->CYCCNT);
    return SysTick->VAL;
}

void teensyStepErrorHandler()
{
#ifdef DEBUG
    __asm("bkpt");
#endif

    while (true)
    {
    }
}
void _Error_Handler(const char *, int)
{
    teensyStepErrorHandler();
}
