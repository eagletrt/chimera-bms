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

const uint8_t InvBusVoltage[8];
const uint8_t TsON[8];
const uint8_t TsOFF[8];

void cells_init(Cell cells[]);

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
