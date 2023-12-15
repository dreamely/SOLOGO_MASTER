/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#include "led_driver.h"
#include "applicationTick.h"
#include "uart.h"
#include "protocol.h"
//#include "flash_rw.h"
#include "eeprom.h"
#include "rtc_driver.h"
#include "lora.h"
#include "modbus.h"

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
#define RUN_LED_Pin GPIO_PIN_2
#define RUN_LED_GPIO_Port GPIOE
#define LORA_RESET_Pin GPIO_PIN_3
#define LORA_RESET_GPIO_Port GPIOE
#define RTC_SCK_Pin GPIO_PIN_0
#define RTC_SCK_GPIO_Port GPIOD
#define RTC_IO_Pin GPIO_PIN_1
#define RTC_IO_GPIO_Port GPIOD
#define RTC_RST_Pin GPIO_PIN_2
#define RTC_RST_GPIO_Port GPIOD
#define TXD_LED_Pin GPIO_PIN_3
#define TXD_LED_GPIO_Port GPIOD
#define RXD_LED_Pin GPIO_PIN_4
#define RXD_LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
extern uint32_t microGetSystemTime(void);
void HAL_WatchDogReload(void);
void ExitTimerSet(void);
uint8_t ExitTimerChekck(void);
void UART_RX_DMA_Handler(void);

#define		TIME_10_MS											10
#define		TIME_30_MS											30
#define		TIME_50_MS											50
#define 	TIME_100_MS											100
#define 	TIME_250_MS											250
#define 	TIME_1200_MS										1200
#define 	TIME_500_MS											500
#define 	TIME_1_SEC											1000
#define 	TIME_5_SEC											5000

#define 	SEC_5_COUNT											20	//5/0.25 = 20

#define 	TIME_30_SEC_COUNT								30
#define 	TIME_30_SEC											30000
#define 	TIME_1_MINUTE										60000
#define 	TIME_1_HOUR			  							3600000

#define 	SENSOR_DATA_SEND_TIME						200

#define BIT(x)                (1 << (x))
#define CLEARBIT(reg, bit)    reg &= ~(BIT(bit))
#define SETBIT(reg, bit)      reg |= BIT(bit)
#define INPUTPIN(reg, bit)    reg &= ~(1<<bit)
#define OUTPUTPIN(reg, bit)   reg |= 1<<bit
#define READBIT(reg, bit)     (reg & (BIT(bit)))
#define READBITS(reg, bits)   (reg & (bits))

#define TRUE									1
#define	FALSE									0

typedef enum SYSTICK_TYPE_enum
{	
	SYSTICK_Delay_0,
	SYSTICK_X_GetByte_2,
	SYSTICK_Messages_timer_tick_5,
	SYSTICK_NUM_INT,
	SYSTICK_NUM_INT1

} SYSTICK_TYPE_t;

//******************************************************
// US system delay
//******************************************************
__STATIC_INLINE void Pol_Delay_us(volatile uint32_t microseconds)
{
	microseconds *= (SystemCoreClock/1000000);
	
	while(microseconds--);
}

void HAL_3usDelay(uint16_t us);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
