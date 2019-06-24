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
typedef enum
{
	CAN_OUT_PACK_VOLTS = 0x01,
	CAN_OUT_PACK_TEMPS = 0x0A,
	CAN_INITIAL_CHECK = 0x02,
	CAN_OUT_TS_ON = 0x03,
	CAN_OUT_TS_OFF = 0x04,
	CAN_OUT_CURRENT = 0x05,
	CAN_OUT_AVG_TEMP = 0x06,
	CAN_OUT_MAX_TEMP = 0x07,
	CAN_OUT_ERROR = 0x08,

	CAN_IN_BUS_VOLTAGE = 0xEB,

	CAN_IN_TS_ON = 0x0A,
	CAN_IN_TS_OFF = 0x0B

} CAN_CODE_T;

typedef enum
{
	CAN_ID_BMS = 0xAA,
	CAN_ID_ECU = 0x55,
	CAN_ID_GUI = 0xA8,
	CAN_ID_IN_INVERTER_L = 0x181,
	CAN_ID_OUT_INVERTER_L = 0x201

} CAN_ID_T;

extern uint8_t CAN_MSG_INVERTER_VOLTAGE[8];
extern uint8_t CAN_MSG_TS_ON[8];
extern uint8_t CAN_MSG_TS_OFF[8];

// CAN filter used during regular use
extern CAN_FilterConfTypeDef CAN_FILTER_NORMAL;

// CAN filter used during precharge cycle
extern CAN_FilterConfTypeDef CAN_FILTER_PRECHARGE;

void can_init(CAN_HandleTypeDef *canh);
void can_init_msg(uint8_t data[8]);
void can_send(CAN_HandleTypeDef *canh, uint8_t data[]);
uint8_t can_receive(CAN_HandleTypeDef *canh, CanRxMsgTypeDef *rx);

bool can_check_error(CAN_HandleTypeDef *canh);

void can_send_current(CAN_HandleTypeDef *canh, int32_t current);
void can_send_pack_voltage(CAN_HandleTypeDef *canh, PACK_T pack);
void can_send_pack_temperature(CAN_HandleTypeDef *canh, PACK_T pack);
void can_send_error(CAN_HandleTypeDef *canh, ERROR_T error, uint8_t index,
					PACK_T *pack);

#endif /* CAN_H_ */
