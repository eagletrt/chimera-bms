/**
 ******************************************************************************
 * @file	can.c
 * @author	Gregor
 * @brief	This file contains some CAN functions to ease data transmission
 ******************************************************************************
 */

#include <can.h>

CAN_FilterConfTypeDef runFilter; // CAN filter used during regular use
CAN_FilterConfTypeDef pcFilter; // CAN filter used during precharge cycle

void CAN_Init(CAN_HandleTypeDef *canh) {
	CAN_Init(canh);

	// CAN Filter Initialization
	runFilter.FilterNumber = 0;
	runFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	runFilter.FilterIdLow = 0x55 << 5;
	runFilter.FilterIdHigh = 0xA8 << 5;
	runFilter.FilterMaskIdHigh = 0x55 << 5;
	runFilter.FilterMaskIdLow = 0x55 << 5;
	runFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	runFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	runFilter.FilterActivation = ENABLE;

	HAL_CAN_ConfigFilter(canh, &runFilter);

	pcFilter.FilterNumber = 0;
	pcFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	pcFilter.FilterIdLow = 0x181 << 5;
	pcFilter.FilterIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdLow = 0x181 << 5;
	pcFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	pcFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	pcFilter.FilterActivation = ENABLE;

}

uint8_t CAN_recv(CAN_HandleTypeDef *canh, CanRxMsgTypeDef*rx) {

	(*canh).pRxMsg = rx;
	return HAL_CAN_Receive(canh, CAN_FIFO0, 1) == HAL_OK;

}

void CAN_filter_precharge(CAN_HandleTypeDef *canh) {
	HAL_CAN_ConfigFilter(canh, &pcFilter);
}
void CAN_filter_normal(CAN_HandleTypeDef *canh) {
	HAL_CAN_ConfigFilter(canh, &runFilter);
}

/**
 * @brief		This function is used to transmit a CAN message
 * @param		STD ID
 * @param		Data lenght
 * @param		Data array
 * @retval		16 bit unsigned integer containing the two PEC bytes
 */
void CAN_Transmit(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC,
		const uint8_t data[]) {

	CanTxMsgTypeDef TxMsg;
	TxMsg.IDE = CAN_ID_STD;
	TxMsg.StdId = id;
	TxMsg.DLC = DLC;
	TxMsg.RTR = CAN_RTR_DATA;

	uint8_t i;
	for (i = 0; i < DLC; i++) {
		TxMsg.Data[i] = data[i];
	}

	canh->pTxMsg = &TxMsg;
	HAL_CAN_Transmit(canh, 2);

}

void CAN_CellErr(CAN_HandleTypeDef *canh, uint8_t index, Cell cell) {

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
