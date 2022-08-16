/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CloseButton_Pin GPIO_PIN_9
#define CloseButton_GPIO_Port GPIOB
#define DoorState_Pin GPIO_PIN_0
#define DoorState_GPIO_Port GPIOA
#define LockState_Pin GPIO_PIN_1
#define LockState_GPIO_Port GPIOA
#define StepperUART_Pin GPIO_PIN_2
#define StepperUART_GPIO_Port GPIOA
#define StepperEnable_Pin GPIO_PIN_3
#define StepperEnable_GPIO_Port GPIOA
#define StepperDiag_Pin GPIO_PIN_4
#define StepperDiag_GPIO_Port GPIOA
#define StepperStep_Pin GPIO_PIN_5
#define StepperStep_GPIO_Port GPIOA
#define StepperDirection_Pin GPIO_PIN_6
#define StepperDirection_GPIO_Port GPIOA
#define ForceOpenInput_Pin GPIO_PIN_7
#define ForceOpenInput_GPIO_Port GPIOA
#define ForceCloseInput_Pin GPIO_PIN_0
#define ForceCloseInput_GPIO_Port GPIOB
#define tempMotor_Pin GPIO_PIN_1
#define tempMotor_GPIO_Port GPIOB
#define inputCurrent_Pin GPIO_PIN_2
#define inputCurrent_GPIO_Port GPIOB
#define LED_Red_Pin GPIO_PIN_15
#define LED_Red_GPIO_Port GPIOA
#define LED_Green_Pin GPIO_PIN_4
#define LED_Green_GPIO_Port GPIOB
#define OpenButton_Pin GPIO_PIN_8
#define OpenButton_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
