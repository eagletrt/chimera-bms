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
void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t ID, uint32_t DLC, const uint8_t data[]) {

	CanTxMsgTypeDef TxMsg;
	TxMsg.IDE = CAN_ID_STD;
	TxMsg.StdId = ID;
	TxMsg.DLC = DLC;
	TxMsg.RTR = CAN_RTR_DATA;

	uint8_t i;
	for (i = 0; i < DLC; i++) {
		TxMsg.Data[i] = data[i];
	}

	canh->pTxMsg = &TxMsg;
	HAL_CAN_Transmit(canh, 2);

}

void CellErr(CAN_HandleTypeDef *canh, uint8_t index, Cell cell) {

	uint8_t data[8];

	switch (cell.state) {
	case CELL_UNDER_VOLTAGE:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x01;
		data[3] = index;
		data[4] = (uint8_t) (cell.voltage >> 8);
		data[5] = (uint8_t) cell.voltage;
		data[6] = 0;
		data[7] = 0;
		break;
	case CELL_OVER_VOLTAGE:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = index;
		data[4] = (uint8_t) (cell.voltage >> 8);
		data[5] = (uint8_t) cell.voltage;
		data[6] = 0;
		data[7] = 0;
		break;
	case CELL_UNDER_TEMPERATURE:
	case CELL_OVER_TEMPERATURE:
		data[0] = 0x08;
		data[1] = 0x03;
		data[2] = 0x02;
		data[3] = index;
		data[4] = (uint8_t) (cell.temperature >> 8);
		data[5] = (uint8_t) cell.temperature;
		data[6] = 0;
		data[7] = 0;
		break;
	case CELL_DATA_NOT_UPDATED:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x00;
		data[3] = index;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		break;

	default:
		return;
	}

	CAN_Transmit(canh, 0xAA, 8, data);
}
