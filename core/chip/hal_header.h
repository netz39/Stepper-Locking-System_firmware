#pragma once

/**
 * @brief Chip "independant" cubeHAL include
 * 
 */
#if __has_include("stm32f4xx_hal.h") 
    #include "stm32f4xx_hal.h"
    #define STM32F4
#else
    #include "stm32f1xx_hal.h"
    #define STM32F1
#endif