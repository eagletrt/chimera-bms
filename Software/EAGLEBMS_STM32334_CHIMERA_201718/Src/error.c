/**
 * @file		error.c
 * @brief		This file contains the functions to handle errors.
 *
 * @date		May 1, 2019
 *
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include <error.h>

/**
 * Reaction times by the rules:
 * 	- 500ms for voltage and current
 * 	- 1s for temperatures
 * We stay 100ms below to be safe
 */
#define LTC6804_PEC_TIMEOUT_COUNT 1000
#define CELL_UNDER_VOLTAGE_TIMEOUT_MS 400
#define CELL_OVER_VOLTAGE_TIMEOUT_MS 400
#define CELL_UNDER_TEMPERATURE_TIMEOUT_MS 900
#define CELL_OVER_TEMPERATURE_TIMEOUT_MS 900
#define OVER_CURRENT_TIMEOUT_MS 500
#define CAN_TIMEOUT_COUNT 10
#define PRECHARGE_TIMEOUT_COUNT 1

/** @brief	Defines the timeout in count or time for each error type */
ERROR_LIMITS_T timeout[ERROR_NUM_ERRORS] = {
	{LTC6804_PEC_TIMEOUT_COUNT, 0},
	{0, CELL_UNDER_VOLTAGE_TIMEOUT_MS},
	{0, CELL_OVER_VOLTAGE_TIMEOUT_MS},
	//{ 0, CELL_UNDER_TEMPERATURE_TIMEOUT_MS },
	{0, CELL_OVER_TEMPERATURE_TIMEOUT_MS},
	{0, OVER_CURRENT_TIMEOUT_MS},
	{CAN_TIMEOUT_COUNT, 0},
	{PRECHARGE_TIMEOUT_COUNT, 0}};

/**
 * @brief	Initializes an error structure
 *
 * @param	error	The error structure to initialize
 */
void error_init(ERROR_STATUS_T *error)
{
	error->type = ERROR_OK;
	error->count = 0;
	error->active = false;
	error->fatal = false;
	error->time_stamp = 0;
}

/**
 * @brief	Activates an error.
 *
 * @param	type	The error type
 * @param	er		The error "instance" to activate
 * @param	now		The current time
 */
void error_set(ERROR_T type, ERROR_STATUS_T *er, uint32_t now)
{
	// If the error is already enabled
	if (er->active)
	{
		// and it's the same error type
		if (er->type == type)
		{
			er->count++;
		}
	}
	else
	{
		er->type = type;
		er->active = true;
		er->time_stamp = now;
		er->count = 1;
	}
}

/**
 * @brief	Deactivates an error.
 *
 * @param	type	The type of error to deactivate
 * @param	er		The error "instance" to deactivate
 */
void error_unset(ERROR_T type, ERROR_STATUS_T *er)
{
	// Disable only if the types are the same. We don't want to disable a
	// different errors
	if (er->type == type)
	{
		er->type = ERROR_OK;
		er->active = false;
		er->fatal = false;
	}
}

/**
 * @brief	Checks if an error has become fatal.
 *
 * @param	error	The error "instance" to check.
 * @param	now		The current time.
 * @param	halt	The error return value.
 */
void error_check_fatal(ERROR_STATUS_T *error, uint32_t now, ERROR_T *halt)
{
	if (error->active)
	{
		if (_error_check_count(error) || _error_check_timeout(error, now))
		{
			error->fatal = true;
			ER_RAISE(halt, error->type);
		}
	}

End:;
}

/**
 * @brief		Checks whether to trigger an error based on number of
 * 					occurrences.
 * @details	This will trigger the error if the number of occurrences exceeds the
 * 					count parameter.
 *
 * @param		error	The error structure to check
 *
 * @retval	true for error, false for OK
 */
bool _error_check_count(ERROR_STATUS_T *error)
{
	if (timeout[error->type].count)
	{
		/** Compares the actual count to the timeout for this error type */
		if (error->count > timeout[error->type].count)
		{
			return true;
		}
	}

	return false;
}

/**
 * @brief		Checks whether to trigger an error based on time.
 * @details	This will trigger the error if the time elapsed between the first
 * 					occurrence of the error and the current time is more than
 * the timeout.
 *
 * @param		error	The error struct to check
 * @param		now		The current time
 *
 * @retval	true for error, false for OK
 */
bool _error_check_timeout(ERROR_STATUS_T *error, uint32_t now)
{
	if (timeout[error->type].timeout)
	{
		if (now - error->time_stamp > timeout[error->type].timeout)
		{
			return true;
		}
	}
	return false;
}
