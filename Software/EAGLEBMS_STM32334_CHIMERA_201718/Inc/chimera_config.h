/**
 * @file		chimera_config.h
 * @brief		This file contains configuration settings for Chimera Evoluzione
 *
 * @date		Dec 12, 2018
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef CHIMERA_CONFIG_H_
#define CHIMERA_CONFIG_H_
#include <stdbool.h>

#define CHARGING 0

#define BOARD_REV volpe

enum {
	LTC6804_COUNT = 12,		/*!< Number of daisy chained LTCs */
	LTC6804_CELL_COUNT = 9, /*!< Number of cells a single IC controls. Refer to
							   cell_distribution for configuration */
	LTC6804_REG_COUNT = 4, /* Number of registers for a single IC. A, B, C, D */
	LTC6804_REG_CELL_COUNT = 3, /* Max number of cells handled by a register.
								   Refer to cell distribution */

	PACK_MODULE_COUNT =
		LTC6804_COUNT * LTC6804_CELL_COUNT,	 // Total number of cells in series

	PACK_MAX_CURRENT = 200,

	CELL_WARN_VOLTAGE = 28000,
	CELL_MIN_VOLTAGE = 25000,
	CELL_MAX_VOLTAGE = 42250,
	CELL_MAX_TEMPERATURE = 6000,
};

/**
 * @details	Defines the cell distribution inside the rdcv groups:
 * 					0: cell not present
 * 					1: cell present
 */
static const bool
	cell_distribution[LTC6804_REG_COUNT * LTC6804_REG_CELL_COUNT] = {
		1, 1, 1,  // GROUP A
		1, 1, 0,  // GROUP B
		1, 1, 1,  // GROUP C
		1, 0, 0	  // GROUP D
};

#endif /* CHIMERA_CONFIG_H_ */
