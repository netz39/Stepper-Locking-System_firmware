#pragma once

#include "core/fault_handler.h"
#include "gpio.h"

inline static void faultHandlerStepperFailsafe()
{
    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_SET);
    faultHandler();
}