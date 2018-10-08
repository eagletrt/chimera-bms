/*
 * can.h
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */

#ifndef CAN_H_
#define CAN_H_

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_can.h"


 int CAN_Send(uint16_t id1,uint16_t id2,uint8_t dataTx[8], int size,CAN_HandleTypeDef hcan );
void Send2GUI(uint16_t data[108],int id,  int size,CAN_HandleTypeDef hcan,float adcV, float current);
int CAN_Receive(uint16_t id,uint32_t CAN_FIFON, int size,CAN_HandleTypeDef hcan );
#endif /* CAN_H_ */
