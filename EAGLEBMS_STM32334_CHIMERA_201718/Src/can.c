/**
 ******************************************************************************
 * @file	can.c
 * @author	Gregor
 * @brief	This file contains some CAN functions to ease data transmission
 ******************************************************************************
 */

#include <can.h>


/**
  * @brief		This function is used to transmit a CAN message
  * @param		STD ID
  * @param		Data lenght
  * @param		Data array
  * @retval		16 bit unsigned integer containing the two PEC bytes
  */
void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t ID, uint32_t DLC, uint8_t data[8]){

	CanTxMsgTypeDef TxMsg;
	TxMsg.IDE = CAN_ID_STD;
	TxMsg.StdId = ID;
	TxMsg.DLC = DLC;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.Data[0] = data[0];
	TxMsg.Data[1] = data[1];
	TxMsg.Data[2] = data[2];
	TxMsg.Data[3] = data[3];
	TxMsg.Data[4] = data[4];
	TxMsg.Data[5] = data[5];
	TxMsg.Data[6] = data[6];
	TxMsg.Data[7] = data[7];
	canh->pTxMsg = &TxMsg;
	HAL_CAN_Transmit(canh, 2);

}

void ErrorMsg(PackStateTypeDef status, uint8_t cell, uint16_t value, uint8_t data[]){

	switch (status) {

	case UNDER_VOLTAGE :
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x01;
		data[3] = cell;
		data[4] = (uint8_t) (value >> 8);
		data[5] = (uint8_t) value;
		data[6] = 0;
		data[7] = 0;
	break;
	case OVER_VOLTAGE :
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = cell;
		data[4] = (uint8_t) (value >> 8);
		data[5] = (uint8_t) value;
		data[6] = 0;
		data[7] = 0;
		break;
	case OVER_TEMPERATURE :
		data[0] = 0x08;
		data[1] = 0x03;
		data[2] = 0x02;
		data[3] = cell;
		data[4] = (uint8_t) (value >> 8);
		data[5] = (uint8_t) value;
		data[6] = 0;
		data[7] = 0;
		break;
	case PACK_OVER_TEMPERATURE :
		data[0] = 0x08;
		data[1] = 0x04;
		data[2] = (uint8_t) (value >> 8);
		data[3] = (uint8_t) value;
		data[4] = 0x00;
		data[5] = 0x00;
		data[6] = 0x00;
		data[7] = 0x00;
		break;

	case DATA_NOT_UPDATED :
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x00;
		data[3] = cell;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		break;

	case PACK_OK :
		break;

	}

}
