/*
 * eagle_vars.h
 *
 *  Created on: Dec 12, 2018
 *      Author: bonnee
 */

#ifndef EAGLE_VARS_H_
#define EAGLE_VARS_H_

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define TOT_IC 12 // number of daisy chain
#define N_REGISTERS 4
#define CELLS_PER_IC 9

#define N_CELLS 108

#define CELL_MIN_VOLTAGE 25000
#define CELL_MAX_VOLTAGE 42250
#define CELL_MAX_TEMPERATURE 7000

#define PACK_MIN_VOLTAGE N_CELLS*CELL_MIN_VOLTAGE
#define PACK_MAX_VOLTAGE N_CELLS*CELL_MAX_VOLTAGE
#define PACK_MAX_TEMPERATURE 6500

uint8_t rdcv_cmd[]={
		0x04,	// A
		0x06,	// B
		0x08,	// C
		0x0A	// D
};

/**
 * Defines the cell distribution inside the rdcv groups
 */
uint8_t cell_distribution[]={
		1,1,1,	// GROUP A
		1,1,0,	// GROUP B
		1,1,1,	// GROUP C
		1,0,0	// GROUP D
};

/** @defgroup PackState Battery Pack Status
 */
typedef enum {
	CELL_OK				  =	0x00U, /*!< Cell is OK */
	CELL_UNDER_VOLTAGE	  =	0x01U, /*!< Cell in Under Voltage */
	CELL_OVER_VOLTAGE 	  =	0x02U, /*!< Cell in Over Voltage */
	CELL_OVER_TEMPERATURE = 0x03U, /*!< Cell in Over Temperature */
	CELL_DATA_NOT_UPDATED = 0x04U, /*!< Data not received form LTC68xx for more than 1000 cycles */
} CellState;

typedef enum {
	PACK_OK 			  =	0x00U,
	PACK_OVER_TEMPERATURE = 0x01U,
	PACK_UNDER_VOLTAGE	  =	0x02U,
	PACK_OVER_VOLTAGE 	  = 0x03U
} PackState;


typedef struct {
	uint16_t voltage;
	uint16_t temperature;

	uint16_t voltage_faults;
	uint16_t temperature_faults;
} Cell;

typedef struct{
	uint32_t voltage;
	uint32_t temperature;
	PackState state;
} Pack;

#endif /* EAGLE_VARS_H_ */
