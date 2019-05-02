/*
 * error_handler.c
 *
 *  Created on: May 1, 2019
 *      Author: bonnee
 */

#include <error.h>

#define LTC6804_PEC_TIMEOUT_COUNT 10
#define CELL_UNDER_VOLTAGE_TIMEOUT_MS 1000
#define CELL_OVER_VOLTAGE_TIMEOUT_MS 1000
#define CELL_UNDER_TEMPERATURE_TIMEOUT_MS 10000
#define CELL_OVER_TEMPERATURE_TIMEOUT_MS 10000
#define	OVER_CURRENT_TIMEOUT_MS 1000
#define CAN_TIMEOUT_COUNT 10
#define PRECHARGE_TIMEOUT_COUNT 1

ERROR_STATUS_T errors[ERROR_NUM_ERRORS];
ERROR_LIMITS_T timeout[ERROR_NUM_ERRORS] =
{
{ LTC6804_PEC_TIMEOUT_COUNT, 0 },
{ 0, CELL_UNDER_VOLTAGE_TIMEOUT_MS },
{ 0, CELL_OVER_VOLTAGE_TIMEOUT_MS },
{ 0, CELL_UNDER_TEMPERATURE_TIMEOUT_MS },
{ 0, CELL_OVER_TEMPERATURE_TIMEOUT_MS },
{ 0, OVER_CURRENT_TIMEOUT_MS },
{ CAN_TIMEOUT_COUNT, 0 },
{ PRECHARGE_TIMEOUT_COUNT, 0 } };

void error_init()
{
	uint8_t i;
	for (i = 0; i < ERROR_NUM_ERRORS; i++)
	{
		errors[i].count = 0;
		errors[i].error = false;
		errors[i].thrown = false;
		errors[i].time_stamp = 0;
	}
}

void error_set(ERROR_T er, uint32_t time_stamp)
{
	if (errors[er].error)
	{
		errors[er].count++;
	}
	else
	{
		errors[er].error = true;
		errors[er].time_stamp = time_stamp;
		errors[er].count = 1;
	}
}

void error_unset(ERROR_T er)
{
	errors[er].error = false;
	errors[er].thrown = false;
}

ERROR_T error_get()
{
	uint8_t i;
	for (i = 0; i < ERROR_NUM_ERRORS; i++)
	{
		if (errors[i].error && errors[i].thrown)
		{
			return i;
		}
	}
	return ERROR_NUM_ERRORS;
}

bool error_check_fatal(uint32_t time_stamp)
{
	bool halt = false;

	uint8_t i;
	for (i = 0; i < ERROR_NUM_ERRORS; i++)
	{
		if (errors[i].error)
		{
			halt = _error_check_count(i, errors[i].count);
			halt = _error_check_timeout(i, errors[i].time_stamp);
			errors[i].thrown = halt;
		}
	}

	return false;//return halt;
}

bool _error_check_count(ERROR_T er, uint16_t count)
{
	if (count)
	{
		if (errors[er].count > timeout[er].count)
		{
			return true;
		}
	}

	return false;
}

bool _error_check_timeout(ERROR_T er, uint32_t time_stamp)
{
	if (time_stamp)
	{
		if (time_stamp - errors[er].time_stamp < timeout[er].timeout)
		{
			return true;
		}
	}
	return false;
}
