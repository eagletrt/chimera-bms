/**
 * @file		bms.c
 * @brief		This file contains the functions to manage the battery pack
 * 					procedures
 *
 * @date		May 21, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "bms.h"
#include <inttypes.h>

#define BMS_PRECHARGE_TIMEOUT 1000
#define BMS_PRECHARGE_BYPASS_TIMEOUT 8000

/**
 * @brief		Sets the given pin and updates the pin's state
 *
 * @param		pin		The pin to set
 * @param		state	The state to set the pin to
 */
void bms_write_pin(BMS_PIN_T *pin, GPIO_PinState state)
{
	pin->state = state;
	HAL_GPIO_WritePin(pin->gpio, pin->id, pin->state);
}

/**
 * @brief		Starts the precharge routine
 *
 * @param		bms		The BMS config structure
 */
void bms_precharge_start(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_ts_on, GPIO_PIN_SET);
	bms->precharge_timestamp = HAL_GetTick();
	bms->status = BMS_PRECHARGE;
}

void bms_precharge_bypass(BMS_CONFIG_T *bms)
{
	bms_precharge_start(bms);
	bms->precharge_bypass = true;
}

/**
 * @brief		Ends precharge if timeouts have expired
 *
 * @param		bms	The BMS config structure
 * @retval	The updated BMS status
 */
BMS_STATUS_T bms_precharge_check(BMS_CONFIG_T *bms)
{
	if (bms->status == BMS_PRECHARGE)
	{

		if (bms->precharge_bypass && HAL_GetTick() - bms->precharge_timestamp >=
										 BMS_PRECHARGE_BYPASS_TIMEOUT)
		{
			bms_precharge_end(bms);
		}
		else if (HAL_GetTick() - bms->precharge_timestamp >=
				 BMS_PRECHARGE_TIMEOUT)
		{
			bms_set_ts_off(bms);
		}
	}
	return bms->status;
}

/**
 * @brief	Succsessfully ends the precharge procedure
 *
 * @param	bms	The BMS config structure
 */
void bms_precharge_end(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_precharge_end, GPIO_PIN_SET);
	HAL_Delay(1);
	bms_write_pin(&bms->pin_precharge_end, GPIO_PIN_RESET);
	bms->status = BMS_ON;
}

/**
 * @brief	Sets the TS_ON pin to low
 *
 * @param	bms	The BMS config structure
 */
void bms_set_ts_off(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_ts_on, GPIO_PIN_RESET);
	bms->status = BMS_OFF;
}

/**
 * @brief	Sets the TS_ON pin to high
 *
 * @param	bms	The BMS config structure
 */
void bms_set_fault(BMS_CONFIG_T *bms)
{
	bms_write_pin(&bms->pin_fault, GPIO_PIN_RESET);
	bms->status = BMS_HALT;
}