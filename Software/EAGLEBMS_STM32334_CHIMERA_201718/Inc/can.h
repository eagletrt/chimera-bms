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
#include <ltc_68xx.h>

void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC, const uint8_t data[]);
void CellErr(CAN_HandleTypeDef *canh, uint8_t index, Cell cell);
#endif /* CAN_H_ */
