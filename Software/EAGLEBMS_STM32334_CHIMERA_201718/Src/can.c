/**
 * @file		can.h
 * @brief		This file contains some CAN functions to ease data transmission
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "can.h"

uint8_t CAN_MSG_INVERTER_VOLTAGE[8] = {0x3D, 0xEB, 0, 0, 0, 0, 0, 0};
uint8_t CAN_MSG_TS_ON[8] = {CAN_OUT_TS_ON, 0, 0, 0, 0, 0, 0, 0};
uint8_t CAN_MSG_TS_OFF[8] = {CAN_OUT_TS_OFF, 0, 0, 0, 0, 0, 0, 0};

// CAN filter used during regular use
CAN_FilterConfTypeDef CAN_FILTER_NORMAL;

// CAN filter used during precharge cycle
CAN_FilterConfTypeDef CAN_FILTER_PRECHARGE;

void can_init_msg(uint8_t data[8])
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		data[i] = 0;
	}
}

void can_init(CAN_HandleTypeDef *canh)
{
	// CAN Filter Initialization
	CAN_FILTER_NORMAL.FilterNumber = 0;
	CAN_FILTER_NORMAL.FilterMode = CAN_FILTERMODE_IDLIST;
	CAN_FILTER_NORMAL.FilterIdLow = 0x55 << 5;
	CAN_FILTER_NORMAL.FilterIdHigh = 0xA8 << 5;
	CAN_FILTER_NORMAL.FilterMaskIdHigh = 0x55 << 5;
	CAN_FILTER_NORMAL.FilterMaskIdLow = 0x55 << 5;
	CAN_FILTER_NORMAL.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FILTER_NORMAL.FilterScale = CAN_FILTERSCALE_16BIT;
	CAN_FILTER_NORMAL.FilterActivation = ENABLE;

	CAN_FILTER_PRECHARGE.FilterNumber = 0;
	CAN_FILTER_PRECHARGE.FilterMode = CAN_FILTERMODE_IDLIST;
	CAN_FILTER_PRECHARGE.FilterIdLow = 0x181 << 5;
	CAN_FILTER_PRECHARGE.FilterIdHigh = 0x181 << 5;
	CAN_FILTER_PRECHARGE.FilterMaskIdHigh = 0x181 << 5;
	CAN_FILTER_PRECHARGE.FilterMaskIdLow = 0x181 << 5;
	CAN_FILTER_PRECHARGE.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FILTER_PRECHARGE.FilterScale = CAN_FILTERSCALE_16BIT;
	CAN_FILTER_PRECHARGE.FilterActivation = ENABLE;

	HAL_CAN_ConfigFilter(canh, &CAN_FILTER_NORMAL);
}

uint8_t can_receive(CAN_HandleTypeDef *canh, CanRxMsgTypeDef *rx)
{
	(*canh).pRxMsg = rx;
	// return HAL_CAN_Receive(canh, CAN_FIFO0, 1) == HAL_OK;
	return HAL_CAN_Receive_IT(canh, CAN_FIFO0) == HAL_OK;
}

bool can_check_error(CAN_HandleTypeDef *canh)
{
	return HAL_CAN_GetState(canh) == HAL_CAN_ERROR_BOF;
}

/**
 * @brief		This function is used to transmit a CAN message
 *
 * @param		canh	The CAN configuration structure
 * @param		data	The data to send
 */
void can_send(CAN_HandleTypeDef *canh, uint8_t data[], size_t size)
{
	CanTxMsgTypeDef tx;

	tx.IDE = CAN_ID_STD;
	tx.StdId = CAN_ID_BMS;
	tx.DLC = size;
	tx.RTR = CAN_RTR_DATA;

	uint8_t i;
	for (i = 0; i < tx.DLC; i++)
	{
		tx.Data[i] = data[i];
	}

	canh->pTxMsg = &tx;
	HAL_CAN_Transmit_IT(canh);
	// HAL_CAN_Transmit(canh, 2);
}

/**
 * @brief		Send current data via CAN
 *
 * @param		canh		CAN configuration structure
 * @param		current	The current variable
 */
void can_send_current(CAN_HandleTypeDef *canh, int32_t current)
{
	// Send current data via CAN

	uint8_t data[4];

	data[0] = CAN_OUT_CURRENT;
	data[1] = current >> 16 & 0xFF;
	data[2] = current >> 8 & 0xFF;
	data[3] = current & 0xFF;
	can_send(canh, data, 4);
}

/**
 * @brief		Send pack data via CAN
 *
 * @param		canh	CAN configuration structure
 * @param		pack	The pack structure with data to send
 */
void can_send_pack_voltage(CAN_HandleTypeDef *canh, PACK_T pack)
{
	uint8_t data[8];

	data[0] = CAN_OUT_PACK_VOLTS;
	data[1] = (uint8_t)(pack.total_voltage >> 16);
	data[2] = (uint8_t)(pack.total_voltage >> 8);
	data[3] = (uint8_t)(pack.total_voltage);
	data[4] = (uint8_t)(pack.avg_temperature >> 8);
	data[5] = (uint8_t)(pack.avg_temperature);
	data[6] = (uint8_t)(pack.min_voltage >> 8);
	data[7] = (uint8_t)(pack.min_voltage);
	/*data[1] = (uint8_t)(pack.total_voltage >> 16);
	data[2] = (uint8_t)(pack.total_voltage >> 8);
	data[3] = (uint8_t)(pack.total_voltage);
	data[4] = (uint8_t)(pack.max_voltagcan_send_ts_on(&hcan);e >> 8);
	data[5] = (uint8_t)(pack.max_voltage);
	data[6] = (uint8_t)(pack.min_voltage >> 8);
	data[7] = (uint8_t)(pack.min_voltage);*/
	can_send(canh, data, 8);
}

/**
 * @brief		Send pack data via CAN
 *
 * @param		canh	CAN configuration structure
 * @param		pack	The pack structure with data to send
 */
void can_send_pack_temperature(CAN_HandleTypeDef *canh, PACK_T pack)
{
	uint8_t data[7];

	data[0] = CAN_OUT_PACK_TEMPS;
	data[1] = (uint8_t)(pack.avg_temperature >> 8);
	data[2] = (uint8_t)(pack.avg_temperature);
	data[3] = (uint8_t)(pack.max_temperature >> 8);
	data[4] = (uint8_t)(pack.max_temperature);
	data[5] = (uint8_t)(pack.min_temperature >> 8);
	data[6] = (uint8_t)(pack.min_temperature);
	can_send(canh, data, 7);
}

/**
 * @brief		Recognise and send errors over can
 *
 * @param		canh	CAN configuration structure
 * @param		error	The error type to send
 * @param		pack	The pack structure with data to send
 */
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, uint8_t index,
					PACK_T *pack)
{
	uint8_t data[5];
	size_t size = 5;

	can_init_msg(data);
	data[0] = CAN_OUT_ERROR;
	data[1] = error;

	switch (error)
	{
	case ERROR_LTC6804_PEC_ERROR:
		data[2] = index;
		size = 3;
		break;
	case ERROR_CELL_UNDER_VOLTAGE:
		data[2] = index;
		data[3] = (uint8_t)(pack->min_voltage >> 8);
		data[4] = (uint8_t)pack->min_voltage;
		break;
	case ERROR_CELL_OVER_VOLTAGE:
		data[2] = index;
		data[3] = (uint8_t)(pack->max_voltage >> 8);
		data[4] = (uint8_t)pack->max_voltage;
		break;
	case ERROR_CELL_OVER_TEMPERATURE:
		data[2] = index;
		data[3] = (uint8_t)(pack->max_temperature >> 8);
		data[4] = (uint8_t)pack->max_temperature;
		break;
	case ERROR_OVER_CURRENT:
		data[2] = pack->current.value >> 16 & 0xFF;
		data[3] = pack->current.value >> 8 & 0xFF;
		data[4] = pack->current.value & 0xFF;
		break;
	default:
		break;
	}

	can_send(canh, data, size);
}
