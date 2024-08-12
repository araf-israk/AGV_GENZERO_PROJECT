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
#include "stm32h7xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IN3_Pin GPIO_PIN_2
#define IN3_GPIO_Port GPIOA
#define IN4_Pin GPIO_PIN_3
#define IN4_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_4
#define IN2_GPIO_Port GPIOA
#define IN1_Pin GPIO_PIN_5
#define IN1_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_8
#define TRIG_GPIO_Port GPIOE
#define TX_EN_Pin GPIO_PIN_13
#define TX_EN_GPIO_Port GPIOB
#define LORA_RX_LED_Pin GPIO_PIN_6
#define LORA_RX_LED_GPIO_Port GPIOD
#define LORA_NSS_Pin GPIO_PIN_10
#define LORA_NSS_GPIO_Port GPIOG
#define LORA_RST_Pin GPIO_PIN_12
#define LORA_RST_GPIO_Port GPIOG
#define LORA_DIO0_Pin GPIO_PIN_13
#define LORA_DIO0_GPIO_Port GPIOG
#define LORA_DIO0_EXTI_IRQn EXTI15_10_IRQn
#define LORA_TX_LED_Pin GPIO_PIN_14
#define LORA_TX_LED_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
