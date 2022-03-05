/**
 * @file		can.h
 * @brief		This file contains some CAN functions to ease data transmission
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef CAN_H_
#define CAN_H_

#include "error.h"
#include "pack.h"

/**
 * Defines CAN message codes
 */
typedef enum {
	CAN_INITIAL_CHECK = 0x02,

	CAN_OUT_PACK_VOLTS = 0x01,
	CAN_OUT_PACK_TEMPS = 0x0A,
	CAN_OUT_TS_ON = 0x03,
	CAN_OUT_TS_OFF = 0x04,
	CAN_OUT_CURRENT = 0x05,
	CAN_OUT_AVG_TEMP = 0x06,
	CAN_OUT_MAX_TEMP = 0x07,
	CAN_OUT_ERROR = 0x08,
	CAN_OUT_WARNING = 0x09,

	CAN_IN_BUS_VOLTAGE = 0xEB,
	CAN_IN_TS_ON = 0x0A,
	CAN_IN_TS_OFF = 0x0B

} CAN_CODE_T;

typedef enum {
	CAN_ID_BMS = 0xAA,
	CAN_ID_ECU = 0x55,
	CAN_ID_IN_INVERTER_L = 0x181,
	CAN_ID_OUT_INVERTER_L = 0x201,
	CAN_ID_IN_INVERTER_R = 0x182,
	CAN_ID_OUT_INVERTER_R = 0x202

} CAN_ID_T;

extern uint8_t CAN_MSG_BUS_VOLTAGE[8];
extern uint8_t CAN_MSG_TS_ON[8];
extern uint8_t CAN_MSG_TS_OFF[8];

extern CAN_FilterTypeDef filter_ecu;
extern CAN_FilterTypeDef filter_inv;

void can_init(CAN_HandleTypeDef *canh);
void can_send(CAN_HandleTypeDef *canh, uint16_t id, uint8_t data[], size_t size);

bool can_check_error(CAN_HandleTypeDef *canh);

void can_send_current(CAN_HandleTypeDef *canh, int16_t current, uint32_t voltage);
void can_send_pack_voltage(CAN_HandleTypeDef *canh, PACK_T pack);
void can_send_pack_temperature(CAN_HandleTypeDef *canh, PACK_T pack);
void can_send_warning(CAN_HandleTypeDef *canh, WARNING_T warning, uint8_t index);
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, uint8_t index, PACK_T *pack);

#endif /* CAN_H_ */
