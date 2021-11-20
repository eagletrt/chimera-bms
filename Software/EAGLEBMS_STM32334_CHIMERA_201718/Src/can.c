/**
 * @file		can.h
 * @brief		This file contains some CAN functions to ease data transmission
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "can.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

uint8_t CAN_MSG_BUS_VOLTAGE[8] = {0x3D, 0xEB, 0, 0, 0, 0, 0, 0};
uint8_t CAN_MSG_TS_ON[8] = {CAN_OUT_TS_ON, 0, 0, 0, 0, 0, 0, 0};
uint8_t CAN_MSG_TS_OFF[8] = {CAN_OUT_TS_OFF, 0, 0, 0, 0, 0, 0, 0};

CAN_FilterConfTypeDef filter_ecu = {.FilterNumber = 0,
									.FilterMode = CAN_FILTERMODE_IDLIST,
									.FilterIdHigh = CAN_ID_ECU << 5,
									.FilterIdLow = 0,
									.FilterMaskIdHigh = 0,
									.FilterMaskIdLow = 0,
									.FilterFIFOAssignment = CAN_FILTER_FIFO0,
									.FilterScale = CAN_FILTERSCALE_16BIT,
									.FilterActivation = ENABLE};

CAN_FilterConfTypeDef filter_inv = {.FilterNumber = 0,
									.FilterMode = CAN_FILTERMODE_IDLIST,
									.FilterIdHigh = CAN_ID_ECU << 5,
									.FilterIdLow = CAN_ID_IN_INVERTER_L << 5,
									.FilterMaskIdHigh = 0,	// CAN_ID_IN_INVERTER_L << 5,
									.FilterMaskIdLow = 0,	// CAN_ID_ECU << 5,
									.FilterFIFOAssignment = CAN_FILTER_FIFO0,
									.FilterScale = CAN_FILTERSCALE_16BIT,
									.FilterActivation = ENABLE};

void can_init(CAN_HandleTypeDef *canh) { HAL_CAN_ConfigFilter(canh, &filter_ecu); }

bool can_check_error(CAN_HandleTypeDef *canh) { return HAL_CAN_GetState(canh) == HAL_CAN_ERROR_BOF; }

/**
 * @brief		This function is used to transmit a CAN message
 *
 * @param		canh	The CAN configuration structure
 * @param		data	The data to send
 */
void can_send(CAN_HandleTypeDef *canh, uint16_t id, uint8_t data[], size_t size) {
	CanTxMsgTypeDef tx = {.IDE = CAN_ID_STD, .StdId = id, .DLC = size, .RTR = CAN_RTR_DATA};

	uint8_t i;
	for (i = 0; i < tx.DLC; i++) {
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
 * @param		current	The current value
 * @param		voltage	The voltage value
 */
void can_send_current(CAN_HandleTypeDef *canh, int16_t current, uint32_t voltage) {
	// Calculate output power (in kW * 10)
	uint16_t power = abs(round(((float)voltage / 1000) * ((float)current / 10) / 10000));
	uint8_t i = 1;

	size_t size = 1 + sizeof(current) + sizeof(power);
	uint8_t data[size];

	data[0] = CAN_OUT_CURRENT;

	data[i++] = (uint8_t)(abs(current) >> 8);
	data[i++] = (uint8_t)abs(current);

	data[i++] = (uint8_t)(power >> 8);
	data[i++] = (uint8_t)(power);

	can_send(canh, CAN_ID_BMS, data, size);
}

/**
 * @brief		Send pack data via CAN
 *
 * @param		canh	CAN configuration structure
 * @param		pack	The pack structure with data to send
 */
void can_send_pack_voltage(CAN_HandleTypeDef *canh, PACK_T pack) {
	size_t size = 8;
	uint8_t data[size];

	data[0] = CAN_OUT_PACK_VOLTS;
	// We only send 24 of the 32 bits of total_voltage
	data[1] = (uint8_t)(pack.total_voltage >> 16);
	data[2] = (uint8_t)(pack.total_voltage >> 8);
	data[3] = (uint8_t)(pack.total_voltage);
	data[4] = (uint8_t)(pack.max_voltage >> 8);
	data[5] = (uint8_t)(pack.max_voltage);
	data[6] = (uint8_t)(pack.min_voltage >> 8);
	data[7] = (uint8_t)(pack.min_voltage);
	can_send(canh, CAN_ID_BMS, data, size);
}

/**
 * @brief		Send pack data via CAN
 *
 * @param		canh	CAN configuration structure
 * @param		pack	The pack structure with data to send
 */
void can_send_pack_temperature(CAN_HandleTypeDef *canh, PACK_T pack) {
	uint8_t data[7] = {0};

	data[0] = CAN_OUT_PACK_TEMPS;
	data[1] = (uint8_t)(pack.avg_temperature >> 8);
	data[2] = (uint8_t)(pack.avg_temperature);
	data[3] = (uint8_t)(pack.max_temperature >> 8);
	data[4] = (uint8_t)(pack.max_temperature);
	data[5] = (uint8_t)(pack.min_temperature >> 8);
	data[6] = (uint8_t)(pack.min_temperature);
	can_send(canh, CAN_ID_BMS, data, 7);
}

/**
 * @brief		Send warnings over CAN
 *
 * @param		canh		CAN configuration structure
 * @param		warning	The warning to send
 * @param		index		The index of the component that generated the
 * 									warning
 */
void can_send_warning(CAN_HandleTypeDef *canh, WARNING_T warning, uint8_t index) {
	static uint32_t timer = 0;

	size_t size = 3;
	uint8_t data[size];

	data[0] = CAN_OUT_WARNING;
	data[1] = warning;
	data[2] = index;

	if (warning != WARN_CELL_LOW_VOLTAGE || (warning == WARN_CELL_LOW_VOLTAGE && HAL_GetTick() - timer >= 500)) {
		timer = HAL_GetTick();
		can_send(canh, CAN_ID_BMS, data, size);
	}
}

/**
 * @brief		Recognise and send errors over CAN
 *
 * @param		canh	CAN configuration structure
 * @param		error	The error type to send
 * @param		index	The index of the component that generated the error
 * @param		pack	The pack structure with data to send
 */
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, uint8_t index, PACK_T *pack) {
	size_t size = 6;
	uint8_t data[size];
	memset(data, 0, size);

	data[0] = CAN_OUT_ERROR;
	data[1] = error;
	data[2] = index;

	switch (error) {
		case ERROR_LTC6804_PEC_ERROR:
			break;

		case ERROR_CELL_UNDER_VOLTAGE:
			data[3] = (uint8_t)(pack->min_voltage >> 8);
			data[4] = (uint8_t)pack->min_voltage;
			break;

		case ERROR_CELL_OVER_VOLTAGE:
			data[3] = (uint8_t)(pack->max_voltage >> 8);
			data[4] = (uint8_t)pack->max_voltage;
			break;

		case ERROR_CELL_OVER_TEMPERATURE:
			data[3] = (uint8_t)(pack->max_temperature >> 8);
			data[4] = (uint8_t)pack->max_temperature;
			break;

		case ERROR_OVER_CURRENT:
			*(typeof(pack->current.value) *)(data + 2) = pack->current.value;
			break;
		default:
			break;
	}

	can_send(canh, CAN_ID_BMS, data, size);
}
