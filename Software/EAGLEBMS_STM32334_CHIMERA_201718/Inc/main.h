/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
*/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <chimera_config.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define CurrentSensor_Pin GPIO_PIN_0
#define CurrentSensor_GPIO_Port GPIOA
#define PreChargeEnd_Pin GPIO_PIN_1
#define PreChargeEnd_GPIO_Port GPIOA
#define CS_6820_Pin GPIO_PIN_3
#define CS_6820_GPIO_Port GPIOA
#define ShutDownStatus_Pin GPIO_PIN_6
#define ShutDownStatus_GPIO_Port GPIOA
#define TS_ON_Pin GPIO_PIN_9
#define TS_ON_GPIO_Port GPIOA
#define BMS_FAULT_Pin GPIO_PIN_10
#define BMS_FAULT_GPIO_Port GPIOA

/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

const uint8_t InvBusVoltage[] = {0x3D, 0xEB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t TsON[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t TsOFF[] = {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void cells_init(Cell *cells,size_t size);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM6_Init(void);

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
