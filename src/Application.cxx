#include "gpio.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

extern "C" void StartDefaultTask(void *)
{

    while (1)
    {
        HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_RESET);

        vTaskDelay(1000);

        HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_Red_GPIO_Port, LED_Red_Pin, GPIO_PIN_SET);

        vTaskDelay(1000);
    }
}