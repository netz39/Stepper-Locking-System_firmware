#if defined(STM32G0B1xx)
#include "TimerField.h"
int TimerField::instances = 0;
// TIM1 is used by HAL for SysTick
TIM_TypeDef *TimerField::timer_mapping[MAX_TIMERS] = {TIM2, TIM4, TIM7};
#endif