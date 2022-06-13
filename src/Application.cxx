#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "main.h"
#include "tim.h"

#include "STM32Step.hpp"

GpioPin stepGpio{StepperStep_GPIO_Port, StepperStep_Pin};
GpioPin dirGpio{StepperDirection_GPIO_Port, StepperDirection_Pin};

TimerArrayControl timerArrayControl(&htim2);

GPIOStepper testStepper(stepGpio, dirGpio);
PositionControl posControl(timerArrayControl);

extern "C" void StartDefaultTask(void *)
{
    posControl.begin();

    testStepper.setAcceleration(1000);
    testStepper.setMaxSpeed(1000);
    testStepper.setTargetRel(8000);

    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);

    posControl.move(testStepper);
    testStepper.setTargetRel(-8000);
    posControl.move(testStepper);

    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);

    while (1)
    {
        // HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);

        vTaskDelay(1000);

        // HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);

        vTaskDelay(1000);
    }
}