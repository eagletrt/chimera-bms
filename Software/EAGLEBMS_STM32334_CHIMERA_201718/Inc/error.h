/*
 * error_handler.h
 *
 *  Created on: May 1, 2019
 *      Author: bonnee
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <inttypes.h>
#include <stdbool.h>

typedef enum
{
	ERROR_LTC6804_PEC_ERROR,

	ERROR_CELL_UNDER_VOLTAGE,
	ERROR_CELL_OVER_VOLTAGE,
	ERROR_CELL_UNDER_TEMPERATURE,
	ERROR_CELL_OVER_TEMPERATURE,

	ERROR_OVER_CURRENT,
	ERROR_CAN,
	ERROR_PRECHARGE_ERROR,

	ERROR_NUM_ERRORS
} ERROR_T;

typedef struct
{
	uint16_t count;
	uint32_t timeout;
} ERROR_LIMITS_T;

typedef struct
{
	bool error;
	bool thrown;
	uint16_t count;
	uint32_t time_stamp;
} ERROR_STATUS_T;

bool _error_check_count(ERROR_T er, uint16_t count);
bool _error_check_timeout(ERROR_T er, uint32_t time_stamp);

void error_set(ERROR_T er,uint32_t time_stamp);
void error_unset(ERROR_T er);
ERROR_T error_get();
bool error_check_fatal(uint32_t time_stamp);

#endif /* ERROR_H_ */
