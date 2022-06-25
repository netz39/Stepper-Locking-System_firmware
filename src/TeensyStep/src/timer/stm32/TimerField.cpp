#if defined(STM32G0B1xx)
#include "TimerField.h"
int TimerField::instances = 0;

TIM_TypeDef *TimerField::timer_mapping[MAX_TIMERS] = {TIM1, TIM4, TIM7};
#endif

// TIM2 and TIM3 is used for PWM LEDs
// TIM6 is used by HAL for SysTick