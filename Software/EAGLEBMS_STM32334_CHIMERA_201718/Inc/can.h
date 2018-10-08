/**
 ******************************************************************************
 * @file	can.h
 * @author	Gregor
 * @brief	This file contains some CAN functions to ease data transmission
 ******************************************************************************
 */

#ifndef CAN_H_
#define CAN_H_

#include <stm32f3xx_hal.h>
#include "ltc_68xx.h"


void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t ID, uint32_t DLC, uint8_t data[8]);
void ErrorMsg(PackStateTypeDef status, uint8_t cell, uint16_t value, uint8_t data[]);

#endif /* CAN_H_ */
