/*
 * chimera_config.h
 *
 *  Created on: Dec 12, 2018
 *      Author: bonnee
 */

#ifndef CHIMERA_CONFIG_H_
#define CHIMERA_CONFIG_H_

#include <inttypes.h>

#define TOT_IC 12 // number of daisy chain
#define CELLS_PER_IC 9	// refer to cell_distribution for arrangement
#define N_REGISTERS 4	// A B C D
#define CELLS_PER_REG 3	// Each register returns a triplet

#define N_CELLS TOT_IC*CELLS_PER_IC

#define CELL_MIN_VOLTAGE 25000
#define CELL_MAX_VOLTAGE 42250
#define CELL_MAX_TEMPERATURE 7000

#define PACK_MIN_VOLTAGE N_CELLS*CELL_MIN_VOLTAGE
#define PACK_MAX_VOLTAGE N_CELLS*CELL_MAX_VOLTAGE
#define PACK_MAX_TEMPERATURE 6500
#define PACK_MIN_TEMPERATURE 1000

static const uint8_t rdcv_cmd[] = {
		0x04,	// A
		0x06,	// B
		0x08,	// C
		0x0A	// D
};

/**
 * Defines the cell distribution inside the rdcv groups
 */
static const uint8_t cell_distribution[] = {
		1,1,1,	// GROUP A
		1,1,0,	// GROUP B
		1,1,1,	// GROUP C
		1,0,0	// GROUP D
};

/**
 * @defgroup single cell status
 */
typedef enum {
	CELL_OK				  =	0x00U,	/*!< Cell is OK */
	CELL_UNDER_VOLTAGE	  =	0x01U,	/*!< Cell Under Voltage */
	CELL_OVER_VOLTAGE 	  =	0x02U,	/*!< Cell Over Voltage */
	CELL_UNDER_TEMPERATURE= 0x03U,
	CELL_OVER_TEMPERATURE = 0x04U,	/*!< Cell Over Temperature */
	CELL_DATA_NOT_UPDATED = 0x05U	/*!< Data not received form LTC68xx for more than 1000 cycles */
} CellState;

/**
 * @defgroup Ok if all cells are ok, error otherwise
 */
typedef enum {
	PACK_OK 				= 0x00U,
	PACK_ERROR				= 0x01U
} PackState;

/**	Cell basic info */
typedef struct {
	uint16_t voltage;			/**< voltage of the cell */
	uint16_t temperature;		/**< temperature of the cell */

	uint8_t voltage_faults;		/**< fault counter for voltage readings */
	uint8_t temperature_faults;	/**< fault counter for temperature readings */

	CellState state;
} Cell;

/** Battery pack basic info */
typedef struct{
	uint32_t voltage;		/**< the total pack voltage */
	uint32_t temperature;	/**< the average temp of all cells of the pack */
	PackState state;		/**< general state of the pack */
} Pack;

#endif /* CHIMERA_CONFIG_H_ */
