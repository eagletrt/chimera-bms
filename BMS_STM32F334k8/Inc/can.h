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


int CAN_Send(int id, uint8_t dataTx[8], int size,CAN_HandleTypeDef hcan);

#endif /* CAN_H_ */
