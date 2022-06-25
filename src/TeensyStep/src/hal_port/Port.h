#pragma once

#include "stm32_def.h"
#include <cstdint>
#include <functional>

#define HIGH 1
#define OUTPUT 1
#define LOW 0

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#define PA5 32
#define PA6 64
#define StepperDirectionPin PA6
#define StepperStepPin PA5

/**
 * Write high or low to a pin. The pin number has to be defined for the specific chip.
 * Additionally it has to be part of the pinToGPIOPinNumber() and pinToGPIOBase() functions.
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
