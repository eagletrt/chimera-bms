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

void CAN_Init(CAN_HandleTypeDef *canh);
void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC,
		const uint8_t data[]);
uint8_t CAN_recv(CAN_HandleTypeDef *canh, CanRxMsgTypeDef*rx);
void CAN_CellErr(CAN_HandleTypeDef *canh, uint8_t index, Cell cell);

void CAN_filter_precharge(CAN_HandleTypeDef *canh);
void CAN_filter_normal(CAN_HandleTypeDef *canh);

#endif /* CAN_H_ */
