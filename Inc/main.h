/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

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
#define CS_W25_Pin GPIO_PIN_4
#define CS_W25_GPIO_Port GPIOE
#define VBAT_CR_Pin GPIO_PIN_8
#define VBAT_CR_GPIO_Port GPIOI
#define Flash_HD_Pin GPIO_PIN_10
#define Flash_HD_GPIO_Port GPIOA
#define Flash_WP_Pin GPIO_PIN_9
#define Flash_WP_GPIO_Port GPIOA
#define IIC_SCL_Pin GPIO_PIN_8
#define IIC_SCL_GPIO_Port GPIOJ
#define IIC_SDA_Pin GPIO_PIN_10
#define IIC_SDA_GPIO_Port GPIOJ
#define CS_MSP5611_Pin GPIO_PIN_10
#define CS_MSP5611_GPIO_Port GPIOF
#define LED_Y_Pin GPIO_PIN_12
#define LED_Y_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_5
#define LED_G_GPIO_Port GPIOJ
#define INT_MPU_Pin GPIO_PIN_4
#define INT_MPU_GPIO_Port GPIOA
#define INT_MPU_EXTI_IRQn EXTI4_IRQn
#define CS_MPU_Pin GPIO_PIN_3
#define CS_MPU_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define LED_Y 		BIT_ADDR(GPIOB_BASE+20,12)//ODR
#define LED_G 		BIT_ADDR(GPIOJ_BASE+20,5)
#define LED_B 		BIT_ADDR(GPIOH_BASE+20,11)
#define CS_MPU 		BIT_ADDR(GPIOA_BASE+20,3)
#define CS_MS 		BIT_ADDR(GPIOF_BASE+20,10)
#define CS_W25 		BIT_ADDR(GPIOE_BASE+20,4)
#define W25_HD 		BIT_ADDR(GPIOA_BASE+20,10)
#define W25_WP 		BIT_ADDR(GPIOA_BASE+20,9)
#define BAT_CTL 	BIT_ADDR(GPIOI_BASE+20,8)
#define NAND_BSY  BIT_ADDR(GPIOG_BASE+16,7)
#define WK_UP			BIT_ADDR(GPIOA_BASE+16,0)

/*IIC1 bus via bit band*/
#define SDA1_IN()  {GPIOJ->MODER&=~(3<<(10*2));GPIOJ->MODER|=0<<10*2;}	//PJ10输入模式
#define SDA1_OUT() {GPIOJ->MODER&=~(3<<(10*2));GPIOJ->MODER|=1<<10*2;} 	//PJ10输出模式
#define IIC1_SCL    BIT_ADDR(GPIOJ_BASE+20,8)
#define IIC1_SDA    BIT_ADDR(GPIOJ_BASE+20,10)
#define READ1_SDA   BIT_ADDR(GPIOJ_BASE+16,10)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
