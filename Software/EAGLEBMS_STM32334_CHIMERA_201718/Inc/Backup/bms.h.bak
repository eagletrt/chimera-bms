/**
 * @file	bms.h
 * @brief	This file contains the functions to manage the battery pack
 * procedures
 *
 * @date	May 21, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef BMS_H_
#define BMS_H_

#include "stm32f3xx_hal.h"
#include <stdbool.h>

typedef enum
{
	BMS_INIT,
	BMS_OFF,
	BMS_PRECHARGE,
	BMS_ON,
	BMS_CHARGE,
	BMS_HALT

} BMS_STATUS_T;

typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t id;
	GPIO_PinState state;

} BMS_PIN_T;

typedef struct
{
	BMS_STATUS_T status;

	BMS_PIN_T pin_fault;
	BMS_PIN_T pin_ts_on;
	BMS_PIN_T pin_chip_select;
	BMS_PIN_T pin_precharge_end;

	uint32_t precharge_timestamp;

} BMS_CONFIG_T;

void bms_write_pin(BMS_PIN_T *pin, GPIO_PinState state);
void bms_precharge_start(BMS_CONFIG_T *bms);
bool bms_precharge_check_timeout(BMS_CONFIG_T *bms);
void bms_precharge_end(BMS_CONFIG_T *bms);
void bms_set_ts_off(BMS_CONFIG_T *bms);
void bms_set_fault(BMS_CONFIG_T *bms);

#endif /* BMS_H_ */