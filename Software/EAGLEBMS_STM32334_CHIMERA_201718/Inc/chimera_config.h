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
	IC_COUNT = 12,							// Number of daisy chained LTCs
	IC_CELL_COUNT = 9,						// Number of cells a single IC controls. Refer to cell_distribution for configuration
	IC_REG_COUNT = 4,						// Number of registers for a single IC. A, B, C, D
	IC_REG_CELL_COUNT = 3,					// Number of cells handled by a register. Refer to cell distribution

	CELL_COUNT = IC_COUNT * IC_CELL_COUNT,	// Total number of cells in series

	CELL_MIN_VOLTAGE = 25000,
	CELL_MAX_VOLTAGE = 42250,
	CELL_MIN_TEMPERATURE = 0000,
	CELL_MAX_TEMPERATURE = 7000,

	PACK_MIN_VOLTAGE = CELL_COUNT * CELL_MIN_VOLTAGE,
	PACK_MAX_VOLTAGE = CELL_COUNT * CELL_MAX_VOLTAGE,
	PACK_MAX_TEMPERATURE = 6500,
	PACK_MIN_TEMPERATURE = 1000
};

/**
 * Defines CAN message codes
 */
typedef enum {
	CAN_PACK_STATE = 0x01,
	CAN_INITIAL_CHECK = 0x02,
	CAN_CONFIRM_START = 0x03,
	CAN_CONFIRM_STOP = 0x04,
	CAN_CURRENT = 0x05,
	CAN_AVG_TEMP = 0x06,
	CAN_MAX_TEMP = 0x07,
	CAN_ERROR = 0x08
} CanCode;

/**
 * Defines the cell distribution inside the rdcv groups.
 * 0: cell not present
 * 1: cell present
 */
static const uint8_t cell_distribution[12] = {
		1, 1, 1,	// GROUP A
		1, 1, 0,	// GROUP B
		1, 1, 1,	// GROUP C
		1, 0, 0		// GROUP D
};

/**
 * @defgroup single cell status
 */
typedef enum {
	CELL_OK = 0x00U, /*!< Cell is OK */
	CELL_UNDER_VOLTAGE = 0x01U, /*!< Cell Under Voltage */
	CELL_OVER_VOLTAGE = 0x02U, /*!< Cell Over Voltage */
	CELL_UNDER_TEMPERATURE = 0x03U,
	CELL_OVER_TEMPERATURE = 0x04U, /*!< Cell Over Temperature */
	CELL_DATA_NOT_UPDATED = 0x05U /*!< Data not received form LTC68xx for more than 1000 cycles */
} CellState;

/**
 * @defgroup Ok if all cells are ok, error otherwise
 */
typedef enum {
	PACK_OK = 0x00U, PACK_ERROR = 0x01U
} PackState;

/**	Cell basic info */
typedef struct {
	uint16_t voltage; /**< voltage of the cell */
	uint16_t temperature; /**< temperature of the cell */

	uint8_t voltage_faults; /**< fault counter for voltage readings */
	uint8_t temperature_faults; /**< fault counter for temperature readings */

	CellState state;
} Cell;

/** Battery pack basic info */
typedef struct {
	uint32_t voltage; /**< the total pack voltage */
	uint16_t temperature; /**< the average temp of all cells of the pack */
	uint16_t max_temperature; /**< the max registered cell temperature */

	int32_t current; /**< the current flowing in or out the pack */
	PackState state; /**< general state of the pack */
} Pack;

#endif /* CHIMERA_CONFIG_H_ */
