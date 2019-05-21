/**
 * @file	can.h
 * @brief	This file contains some CAN functions to ease data transmission
 *
 * @author	Gregor
 * @author  Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "can.h"

const uint8_t CAN_MSG_INVERTER_VOLTAGE[8] = {0x3D, 0xEB, 0, 0, 0, 0, 0, 0};
const uint8_t CAN_MSG_TS_ON[8] = {CAN_OUT_TS_ON, 0, 0, 0, 0, 0, 0, 0};
const uint8_t CAN_MSG_TS_OFF[8] = {CAN_OUT_TS_OFF, 0, 0, 0, 0, 0, 0, 0};

CAN_FilterConfTypeDef runFilter; // CAN filter used during regular use
CAN_FilterConfTypeDef pcFilter;	// CAN filter used during precharge cycle

void can_init_msg(uint8_t data[8])
{
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
	{
		data[i] = 0;
	}
}

void can_init(CAN_HandleTypeDef *canh)
{
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

	pcFilter.FilterNumber = 0;
	pcFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	pcFilter.FilterIdLow = 0x181 << 5;
	pcFilter.FilterIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdLow = 0x181 << 5;
	pcFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	pcFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	pcFilter.FilterActivation = ENABLE;

	can_filter_normal(canh);
}

uint8_t can_receive(CAN_HandleTypeDef *canh, CanRxMsgTypeDef *rx)
{
	(*canh).pRxMsg = rx;
	return HAL_CAN_Receive(canh, CAN_FIFO0, 1) == HAL_OK;
}

void can_filter_precharge(CAN_HandleTypeDef *canh)
{
	HAL_CAN_ConfigFilter(canh, &pcFilter);
}
void can_filter_normal(CAN_HandleTypeDef *canh)
{
	HAL_CAN_ConfigFilter(canh, &runFilter);
}

uint8_t can_check_error(CAN_HandleTypeDef *canh)
{
	if (HAL_CAN_GetState(canh) == HAL_CAN_ERROR_BOF)
	{
		return 1;
	}
	return 0;
}

/**
 * @brief		This function is used to transmit a CAN message
 * @param		STD ID
 * @param		Data lenght
 * @param		Data array
 * @retval		16 bit unsigned integer containing the two PEC bytes
 */
void can_send(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC,
							const uint8_t data[])
{

	CanTxMsgTypeDef TxMsg;
	TxMsg.IDE = CAN_ID_STD;
	TxMsg.StdId = id;
	TxMsg.DLC = DLC;
	TxMsg.RTR = CAN_RTR_DATA;

	uint8_t i;
	for (i = 0; i < DLC; i++)
	{
		TxMsg.Data[i] = data[i];
	}

	canh->pTxMsg = &TxMsg;
	HAL_CAN_Transmit(canh, 2);
}

void can_request_inverter_voltage(CAN_HandleTypeDef *canh)
{
	can_send(canh, CAN_ID_OUT_INVERTER_L, 3, CAN_MSG_INVERTER_VOLTAGE);
}

void can_send_ts_off(CAN_HandleTypeDef *canh)
{
	can_send(canh, CAN_ID_BMS, 8, CAN_MSG_TS_OFF);
}

void can_send_ts_on(CAN_HandleTypeDef *canh)
{
	can_send(canh, CAN_ID_BMS, 8, CAN_MSG_TS_ON);
}

/**
 * @brief	Send current data via CAN
 *
 * @param	canh	CAN configuration structure
 * @param	current	The current variable
 */
void can_send_current(CAN_HandleTypeDef *canh, int32_t current)
{
	// Send current data via CAN

	uint8_t data[8];

	data[0] = CAN_OUT_CURRENT;
	data[1] = (int8_t)(current >> 16);
	data[2] = (int8_t)(current >> 8);
	data[3] = (int8_t)current;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	can_send(canh, 0xAA, 8, data);
}

/**
 * @brief	Send pack data via CAN
 *
 * @param	canh	CAN configuration structure
 * @param	pack	The pack structure with data to send
 */
void can_send_pack_voltage(CAN_HandleTypeDef *canh, PACK_T pack)
{
	uint8_t data[8];

	data[0] = CAN_OUT_PACK_VOLTS;
	data[1] = (uint8_t)(pack.total_voltage >> 16);
	data[2] = (uint8_t)(pack.total_voltage >> 8);
	data[3] = (uint8_t)(pack.total_voltage);
	data[4] = (uint8_t)(pack.max_voltage >> 8);
	data[5] = (uint8_t)(pack.max_voltage);
	data[6] = (uint8_t)(pack.min_voltage >> 8);
	data[7] = (uint8_t)(pack.min_voltage);
	can_send(canh, 0xAA, 8, data);
}

/**
 * @brief	Send pack data via CAN
 *
 * @param	canh	CAN configuration structure
 * @param	pack	The pack structure with data to send
 */
void can_send_pack_temperature(CAN_HandleTypeDef *canh, PACK_T pack)
{
	uint8_t data[8];

	data[0] = CAN_OUT_PACK_TEMPS;
	data[1] = (uint8_t)(pack.avg_temperature >> 8);
	data[2] = (uint8_t)(pack.avg_temperature);
	data[3] = (uint8_t)(pack.max_temperature >> 8);
	data[4] = (uint8_t)(pack.max_temperature);
	data[5] = (uint8_t)(pack.min_temperature >> 8);
	data[6] = (uint8_t)(pack.min_temperature);
	data[7] = 0;
	can_send(canh, 0xAA, 8, data);
}

/**
 * @brief	Recognise and send errors over can
 *
 * @param	canh	CAN configuration structure
 * @param	error	The error type to send
 * @param	pack	The pack structure with data to send
 */
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, PACK_T *pack)
{
	uint8_t data[8];

	switch (error)
	{
	case ERROR_LTC6804_PEC_ERROR:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0;
		data[3] = 0; // Should be ltc index
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		break;
	case ERROR_CELL_UNDER_VOLTAGE:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x01;
		data[3] = 0; // Should be cell index
		data[4] = (uint8_t)(pack->min_voltage >> 8);
		data[5] = (uint8_t)pack->min_voltage;
		data[6] = 0;
		data[7] = 0;
		break;
	case ERROR_CELL_OVER_VOLTAGE:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = 0; // Should be cell index
		data[4] = (uint8_t)(pack->max_voltage >> 8);
		data[5] = (uint8_t)pack->max_voltage;
		data[6] = 0;
		data[7] = 0;
		break;
		/*case ERROR_CELL_UNDER_TEMPERATURE:
		 data[0] = 0x08;
		 data[1] = 0x03;
		 data[2] = 0x02;
		 data[3] = 0;	// Should be cell index
		 data[4] = (uint8_t) (pack->min_temperature >> 8);
		 data[5] = (uint8_t) pack->min_temperature;
		 data[6] = 0;
		 data[7] = 0;
		 break;*/
	case ERROR_CELL_OVER_TEMPERATURE:
		data[0] = 0x08;
		data[1] = 0x03;
		data[2] = 0x02;
		data[3] = 0; // Should be cell index
		data[4] = (uint8_t)(pack->max_temperature >> 8);
		data[5] = (uint8_t)pack->max_temperature;
		data[6] = 0;
		data[7] = 0;
		break;
	case ERROR_OVER_CURRENT:
		break;
	case ERROR_PRECHARGE_ERROR:
		break;
	default:
		break;
	}

	can_send(canh, 0xAA, 8, data);
}
