/**
 * @file		error.h
 * @brief		This file contains the functions to handle errors.
 *
 * @date		May 1, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <inttypes.h>
#include <stdbool.h>

/**
 * @brief		Checks if an error has been triggered, if true it jumps to the
 * 					"End" label
 * @details	This macro checks if a function raised an error. You should call
 * 					this right after a function that can raise some type of
 * 					error.
 */
#define ER_CHK(ST_P)                                                           \
	/*1*/ {                                                                    \
		if (*ST_P != ERROR_OK)                                                 \
		{                                                                      \
			/*3*/ goto End;                                                    \
		}                                                                      \
	}

/** @brief		Sets the error type and jumps to the "End" label
 *	@details	This macro should be called inside a function that can generate
 *						an error. It's used to set the error variable and to
 *						break execution of the function
 */
#define ER_RAISE(ST_P, ST)                                                     \
	/*1*/ {                                                                    \
		*ST_P = ST;                                                            \
		/*3*/ goto End;                                                        \
	}

typedef enum
{
	ERROR_LTC6804_PEC_ERROR,

	ERROR_CELL_UNDER_VOLTAGE,
	ERROR_CELL_OVER_VOLTAGE,
	// ERROR_CELL_UNDER_TEMPERATURE,	// not fatal
	ERROR_CELL_OVER_TEMPERATURE,

	ERROR_OVER_CURRENT,
	ERROR_CAN,
	// ERROR_PRECHARGE_ERROR,	// not fatal

	ERROR_NUM_ERRORS,
	ERROR_OK
} ERROR_T;

/** @brief	Defines the acceptable thresholds over which an error becomes
 * 					critical. Can be a count based limit, a time based one or
 * 					both. 0 values are ignored
 */
typedef struct
{
	uint16_t count;
	uint32_t timeout;
} ERROR_LIMITS_T;

/** @brief	Defines an error instance */
typedef struct
{
	ERROR_T type;		 /*!< Defines the type of error */
	bool active;		 /*!< True if the error is currently happening */
	bool fatal;			 /*!< True if the error is fatal */
	uint16_t count;		 /*!< How many times the error has occurred */
	uint32_t time_stamp; /*!< Last time the error occurred */
} ERROR_STATUS_T;

/** @brief	tuple of value-error_status. Used to store values that can trigger
 * 					an error
 */
typedef struct
{
	uint16_t value;
	ERROR_STATUS_T error;
} ER_UINT16_T;

/** @brief	tuple of value-error_status. Used to store values that can trigger
 * 					an error
 */
typedef struct
{
	int16_t value;
	ERROR_STATUS_T error;
} ER_INT16_T;

/** @brief	tuple of value-error_status. Used to store values that can trigger
 * 					an error
 */
typedef struct
{
	int32_t value;
	ERROR_STATUS_T error;
} ER_INT32_T;

bool _error_check_count(ERROR_STATUS_T *error);
bool _error_check_timeout(ERROR_STATUS_T *error, uint32_t time);

void error_init(ERROR_STATUS_T *error);
void error_set(ERROR_T type, ERROR_STATUS_T *error, uint32_t time_stamp);
void error_unset(ERROR_T type, ERROR_STATUS_T *error);
ERROR_T error_check_fatal(ERROR_STATUS_T *error, uint32_t now);

#endif /* ERROR_H_ */
