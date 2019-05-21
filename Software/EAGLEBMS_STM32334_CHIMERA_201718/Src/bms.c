/**
 * @file	bms.c
 * @brief	This file contains the functions to manage the battery pack procedures
 *
 * @date	May 21, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
*/

#include <inttypes.h>
#include "bms.h"

void bms_write_pin(BMS_PIN_T *pin, GPIO_PinState state)
{
	pin->state = state;
	HAL_GPIO_WritePin(pin->gpio, pin->id, pin->state);
}

void bms_precharge_start(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_ts_on, GPIO_PIN_SET);
	bms->precharge_timestamp = HAL_GetTick();
	bms->status = BMS_PRECHARGE;
}

/**
 * @brief		Checks if precharge should end
 * @retval	Wether precharge is done or not
*/
bool bms_precharge_check_timeout(BMS_CONFIG_T *bms)
{
	if (bms->status == BMS_PRECHARGE && HAL_GetTick() - bms->precharge_timestamp >= 450)
	{
		bms_set_ts_off(bms);
		return true;
	}
	return false;
}

void bms_precharge_end(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_precharge_end, GPIO_PIN_SET);
	HAL_Delay(1);
	bms_write_pin(&bms->pin_precharge_end, GPIO_PIN_RESET);
	bms->status = BMS_ON;
}

void bms_set_ts_off(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_ts_on, GPIO_PIN_RESET);
	bms->status = BMS_OFF;
}

void bms_set_fault(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_fault, GPIO_PIN_SET);
	bms->status = BMS_HALT;
}