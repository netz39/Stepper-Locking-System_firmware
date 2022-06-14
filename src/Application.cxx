#include "FreeRTOS.h"
#include "task.h"

#include "gpio.h"
#include "main.h"

#include "TeensyStep/src/TeensyStep.h"

StepControl stepControl;
Stepper testStepper(StepperStepPin, StepperDirectionPin);

extern "C" void StartDefaultTask(void *)
{
    testStepper.setAcceleration(10000);
    testStepper.setMaxSpeed(5000);

    constexpr auto StepsPerFullRevolution = 200;
    constexpr auto MicroStepsPerStep = 8;
    constexpr auto TearDown = 1;
    constexpr auto NumberRevolutions = 1;

    constexpr auto StepsNeeded =
        NumberRevolutions * TearDown * MicroStepsPerStep * StepsPerFullRevolution;

    HAL_GPIO_WritePin(StepperEnable_GPIO_Port, StepperEnable_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);

    testStepper.setTargetRel(StepsNeeded);
    stepControl.move(testStepper);
    // testStepper.setTargetRel(-StepsNeeded);
    // stepControl.move(testStepper);

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