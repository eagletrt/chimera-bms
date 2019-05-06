/**
 * @file	pack.h
 * @brief	This file contains the functions to manage the battery pack
 *
 * @date	Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
*/

#ifndef PACK_H_
#define PACK_H_

#include <inttypes.h>
#include <stdbool.h>
#include "ltc6804.h"
#include "chimera_config.h"
#include "error.h"

/** @brief Battery pack basic info */
typedef struct
{
	ER_UINT16_T voltages[PACK_MODULE_COUNT];
	ER_UINT16_T temperatures[PACK_MODULE_COUNT];

	uint32_t total_voltage;		/*!< Total pack voltage */
	uint16_t max_voltage;	/*!< Maximum cell voltage */
	uint16_t min_voltage;	/*!< Minimum cell voltage */

	uint16_t avg_temperature; /*!< Average temperature of all the cells of the pack */
	uint16_t max_temperature; /*!< Highest registered cell temperature */
	uint16_t min_temperature; /*!< Smallest cell temperature */

	ER_INT32_T current;	/*!< Instant current draw. Expressed in Ampere * 10 */
} PACK_T;

/**
 * @details Defines the cell distribution inside the rdcv groups:
 * 				0: cell not present
 * 				1: cell present
 */
static const bool cell_distribution[LTC6804_REG_COUNT * LTC6804_REG_CELL_COUNT] =
{
		1, 1, 1,	// GROUP A
		1, 1, 0,	// GROUP B
		1, 1, 1,	// GROUP C
		1, 0, 0		// GROUP D
};

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack);
void pack_update_voltages(SPI_HandleTypeDef *spi, ER_UINT16_T *voltages, ERROR_T *error);
void pack_update_temperatures(SPI_HandleTypeDef *spi, ER_UINT16_T *temperatures, ERROR_T *error);
void pack_update_current(PACK_T *pack, ERROR_T *error);
void pack_update_status(PACK_T *pack);

#endif /* PACK_H_ */
