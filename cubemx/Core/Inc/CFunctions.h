#pragma once
#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void hardwareTimerUpdateCallbackWrapper(TIM_HandleTypeDef* htim);
#ifdef __cplusplus
}
#endif