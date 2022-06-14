#include "Port.h"

GPIO_TypeDef *pinToGPIOBase(int8_t pin)
{
#ifdef STM32F103xB
    switch (pin)
    {
    case PA3:
        return GPIOA;
    case PB0:
    case PB7:
        return GPIOB;
    default:
        teensyStepErrorHandler();
        break;
    }
#endif
#ifdef STM32F103xE
    switch (pin)
    {
    case PA15:
        return GPIOA;
    case PC10: // intentionally fall through
    case PC15:
        return GPIOC;
    default:
        teensyStepErrorHandler();
        break;
    }
    return GPIOA; // will never be hit -> reduce compile warnings
#endif
}

uint16_t pinToGPIOPinNumber(int8_t pin)
{
#ifdef STM32F103xB
    uint16_t pinNumber = 0;
    switch (pin)
    {
    case PA3:
        pinNumber = GPIO_PIN_3;
        break;
    case PB0:
        pinNumber = GPIO_PIN_0;
        break;
    case PB7:
        pinNumber = GPIO_PIN_7;
        break;
    default:
        teensyStepErrorHandler();
        break;
    }
    return pinNumber;
#endif

#ifdef STM32F103xE
    uint16_t pinNumber = 0;
    switch (pin)
    {
    case PA15:
        pinNumber = GPIO_PIN_15;
        break;
    case PC10:
        pinNumber = GPIO_PIN_10;
        break;
    case PC15:
        pinNumber = GPIO_PIN_15;
        break;
    default:
        teensyStepErrorHandler();
        break;
    }
    return pinNumber;
#endif
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
    return (DWT->CYCCNT);
}

void teensyStepErrorHandler()
{
    while (true)
    {
#ifdef STM32F103xB
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8); // let red led flash
#endif
#ifdef STM32F103xE
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2); // let red led flash
#endif
//        taskDISABLE_INTERRUPTS();
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
