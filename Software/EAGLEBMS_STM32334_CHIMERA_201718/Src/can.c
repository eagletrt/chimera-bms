/**
 ******************************************************************************
 * @file	can.c
 * @author	Gregor
 * @brief	This file contains some CAN functions to ease data transmission
 ******************************************************************************
 */

#include <can.h>
#include "ltc6804.h"

static const uint8_t CAN_INV_BUS_VOLTAGE[] =
{ 0x3D, 0xEB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t CAN_TS_ON[] =
{ CAN_CONFIRM_START, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t CAN_TS_OFF[] =
{ CAN_CONFIRM_STOP, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

CAN_FilterConfTypeDef runFilter; // CAN filter used during regular use
CAN_FilterConfTypeDef pcFilter; // CAN filter used during precharge cycle

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

uint8_t can_receive(CAN_HandleTypeDef *canh, CanRxMsgTypeDef*rx)
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
//	if (HAL_CAN_GetState(canh) == HAL_CAN_ERROR_BOF)
//	{
//		return 1;
//	}
	return 0;
}

/**
 * @brief		This function is used to transmit a CAN message
 * @param		STD ID
 * @param		Data lenght
 * @param		Data array
 * @retval		16 bit unsigned integer containing the two PEC bytes
 */
void can_transmit(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC,
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
	can_transmit(canh, 0x201, 3, CAN_INV_BUS_VOLTAGE);
}

void can_send_ts_off(CAN_HandleTypeDef *canh)
{
	can_transmit(canh, 0xAA, 8, CAN_TS_OFF);
}

void can_send_ts_on(CAN_HandleTypeDef *canh)
{
	can_transmit(canh, 0xAA, 8, CAN_TS_ON);
}

void can_send_current(CAN_HandleTypeDef *canh, int32_t current)
{
	// Send current data via CAN

	uint8_t data[8];

	data[0] = CAN_CURRENT;
	data[1] = (int8_t) (current >> 16);
	data[2] = (int8_t) (current >> 8);
	data[3] = (int8_t) current;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	can_transmit(canh, 0xAA, 8, data);
}

void can_send_pack_state(CAN_HandleTypeDef *canh, PACK_T pack)
{
	// Send pack data via CAN

	uint8_t data[8];

	data[0] = CAN_PACK_STATE;
	data[1] = (uint8_t) (pack.voltage >> 16);
	data[2] = (uint8_t) (pack.voltage >> 8);
	data[3] = (uint8_t) (pack.voltage);
	data[4] = (uint8_t) (pack.temperature >> 8);
	data[5] = (uint8_t) (pack.temperature);
	data[6] = (uint8_t) (pack.max_temperature >> 8);
	data[7] = (uint8_t) (pack.max_temperature);
	can_transmit(canh, 0xAA, 8, data);
}

void can_send_ltc_error(CAN_HandleTypeDef *canh, uint8_t index, LTC6804_T ltc)
{
	uint8_t data[8];

	switch (ltc.status)
	{
	case LTC6804_STATUS_OK:
	case LTC6804_STATUS_NONE:
	case LTC6804_STATUS_CELL_ERROR:
		break;
	case LTC6804_STATUS_PEC_ERROR:
		data[0] = 0x08;
		data[1] = 0x02;
		data[2] = 0x00;
		data[3] = index;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		break;
	}

	can_transmit(canh,0xAA,8,data);
}

void can_send_cell_error(CAN_HandleTypeDef *canh, uint8_t index, CELL_T cell)
{

	uint8_t data[8];

	switch (cell.state)
	{
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

	default:
		return;
	}

	can_transmit(canh, 0xAA, 8, data);
}
