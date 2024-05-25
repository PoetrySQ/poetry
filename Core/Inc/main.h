/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define CT1_Pin GPIO_PIN_0
#define CT1_GPIO_Port GPIOA
#define CT2_Pin GPIO_PIN_1
#define CT2_GPIO_Port GPIOA
#define Vpv_Pin GPIO_PIN_2
#define Vpv_GPIO_Port GPIOA
#define Ipv_Pin GPIO_PIN_3
#define Ipv_GPIO_Port GPIOA
#define Vinv_Pin GPIO_PIN_4
#define Vinv_GPIO_Port GPIOA
#define Vg_Pin GPIO_PIN_5
#define Vg_GPIO_Port GPIOA
#define Ig_Pin GPIO_PIN_6
#define Ig_GPIO_Port GPIOA
#define LEDR_Pin GPIO_PIN_0
#define LEDR_GPIO_Port GPIOB
#define LEDG_Pin GPIO_PIN_1
#define LEDG_GPIO_Port GPIOB
#define AC_Relay_Pin GPIO_PIN_10
#define AC_Relay_GPIO_Port GPIOB
#define reverseBridge_Pin GPIO_PIN_14
#define reverseBridge_GPIO_Port GPIOB
#define forwardBridge_Pin GPIO_PIN_15
#define forwardBridge_GPIO_Port GPIOB
#define PWM1H_Pin GPIO_PIN_8
#define PWM1H_GPIO_Port GPIOA
#define PWM1L_Pin GPIO_PIN_9
#define PWM1L_GPIO_Port GPIOA
#define PWM2H_Pin GPIO_PIN_10
#define PWM2H_GPIO_Port GPIOA
#define PWM2L_Pin GPIO_PIN_11
#define PWM2L_GPIO_Port GPIOA
#define SD_Pin GPIO_PIN_3
#define SD_GPIO_Port GPIOB
#define SD2_Pin GPIO_PIN_4
#define SD2_GPIO_Port GPIOB
#define SD1_Pin GPIO_PIN_5
#define SD1_GPIO_Port GPIOB
#define test_Pin GPIO_PIN_7
#define test_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
