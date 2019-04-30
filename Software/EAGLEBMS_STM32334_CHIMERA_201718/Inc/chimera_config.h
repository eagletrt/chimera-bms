/*
 * chimera_config.h
 *
 *  Created on: Dec 12, 2018
 *      Author: bonnee
 */

#ifndef CHIMERA_CONFIG_H_
#define CHIMERA_CONFIG_H_

#include <inttypes.h>

enum {
	LTC6804_COUNT = 12,							// Number of daisy chained LTCs
	LTC6804_CELL_COUNT = 9,						// Number of cells a single IC controls. Refer to cell_distribution for configuration
	LTC6804_REG_COUNT = 4,						// Number of registers for a single IC. A, B, C, D
	LTC6804_REG_CELL_COUNT = 3,					// Max number of cells handled by a register. Refer to cell distribution

	PACK_CELL_COUNT = LTC6804_COUNT * LTC6804_CELL_COUNT,	// Total number of cells in series

	CELL_MIN_VOLTAGE = 25000,
	CELL_MAX_VOLTAGE = 42250,
	CELL_MIN_TEMPERATURE = 0,
	CELL_MAX_TEMPERATURE = 7000,

	PACK_MIN_VOLTAGE = PACK_CELL_COUNT * CELL_MIN_VOLTAGE,
	PACK_MAX_VOLTAGE = PACK_CELL_COUNT * CELL_MAX_VOLTAGE,
	PACK_MAX_TEMPERATURE = 6500,
	PACK_MIN_TEMPERATURE = 1000
};

typedef enum{
	BMS_INIT,
	BMS_OFF,
	BMS_PRECHARGE,
	BMS_ON,
	BMS_CAN_ERROR,
	BMS_LTC6804_ERROR,
	BMS_PACK_ERROR,
	BMS_PRECHARGE_ERROR
} BMS_STATUS_T;

#endif /* CHIMERA_CONFIG_H_ */
