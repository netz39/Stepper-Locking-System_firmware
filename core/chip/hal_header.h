#pragma once

/**
 * @brief Chip "independant" cubeHAL include
 *
 */
#if __has_include("stm32f4xx_hal.h")
#include "stm32f4xx_hal.h"
#define STM32F4
#elif __has_include("stm32f1xx_hal.h")
#include "stm32f1xx_hal.h"
#define STM32F1
#elif __has_include("stm32g0xx_hal.h")
#include "stm32g0xx_hal.h"
#define STM32G0
#endif