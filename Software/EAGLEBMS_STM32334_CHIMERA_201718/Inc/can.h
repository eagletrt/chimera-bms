/**
 * @file	can.h
 * @brief	This file contains some CAN functions to ease data transmission
 *
 * @author	Gregor
 * @author  Matteo Bonora [matteo.bonora@studenti.unitn.it]
*/

#ifndef CAN_H_
#define CAN_H_

#include "pack.h"
#include "error.h"

/**
 * Defines CAN message codes
 */
typedef enum
{
	CAN_PACK_STATE = 0x01,
	CAN_INITIAL_CHECK = 0x02,
	CAN_CONFIRM_START = 0x03,
	CAN_CONFIRM_STOP = 0x04,
	CAN_CURRENT = 0x05,
	CAN_AVG_TEMP = 0x06,
	CAN_MAX_TEMP = 0x07,
	CAN_ERROR = 0x08,

	CAN_CTRL_ID = 0x55,
	CAN_CTRL_TS_ON = 0x0A,
	CAN_CTRL_TS_OFF = 0x0B
} CAN_CODE_T;

void can_init(CAN_HandleTypeDef *canh);
void can_send(CAN_HandleTypeDef *canh, uint32_t id, uint32_t DLC,
		const uint8_t data[]);
uint8_t can_receive(CAN_HandleTypeDef *canh, CanRxMsgTypeDef*rx);

void can_send_current(CAN_HandleTypeDef *canh, int32_t current);
void can_send_pack_state(CAN_HandleTypeDef *canh, PACK_T pack);
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, PACK_T *pack);

void can_request_inverter_voltage(CAN_HandleTypeDef *canh);
void can_send_ts_off(CAN_HandleTypeDef *canh);
void can_send_ts_on(CAN_HandleTypeDef *canh);

void can_filter_precharge(CAN_HandleTypeDef *canh);
void can_filter_normal(CAN_HandleTypeDef *canh);

#endif /* CAN_H_ */
