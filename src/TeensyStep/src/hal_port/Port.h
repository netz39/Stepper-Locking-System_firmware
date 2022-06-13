#pragma once

#include "stm32_def.h"
#include <cstdint>
#include <functional>

#define HIGH 1
#define OUTPUT 1
#define LOW 0
#define LED_BUILTIN 2

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#ifdef STM32F103xB
#define PA3 13
#define PB0 18
#define PB7 43
#define StepperEnablePin PA3
#define StepperDirectionPin PB0
#define StepperStepPin PB7
#endif

#ifdef STM32F103xE
#define PC15 4
#define PC10 51
#define PA15 50
#define StepperEnablePin PC15
#define StepperDirectionPin PC10
#define StepperStepPin PA15
#endif

/**
 * Write high or low to a pin. The pin number has to be defined for the specific chip. Additionally
 * it has to be part of the pinToGPIOPinNumber() and pinToGPIOBase() functions.
 * @param pin Pin for which the polarity has to be set.
 * @param polarity polarity to set, either 0 or 1.
 */
void digitalWrite(int8_t pin, uint8_t polarity);

/**
 * Toggle a pins state. The pin number has to be defined for the specific chip. Additionally
 * it has to be part of the pinToGPIOPinNumber() and pinToGPIOBase() functions.
 * @param pin Pin to toggle.
 */
void digitalToggle(int8_t pin);

/**
 * Delay in milliseconds
 * @param ms time in milliseconds
 */
void delay(uint32_t ms);

/**
 * Function to get the current state of the cycle count register.
 * @return value of cycle count register CYCCNT
 */
uint32_t dwt_getCycles();

/**
 * In Arduino this function is used to set the Pin as INPUT, INPUT_PULLUP, INPUT_PULLDOWN or OUTPUT
 * This is already done via the cubemx project. This function therefore does nothing.
 * @param pin unused
 * @param polarity unused
 */
void pinMode(int8_t pin, uint8_t polarity);

/**
 * Function to be called in case of an error, lets red led flash
 */
void teensyStepErrorHandler();
typedef std::function<void(void)> callback_function_t;
