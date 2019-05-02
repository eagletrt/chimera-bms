/*
 * pack.h
 *
 *  Created on: Apr 11, 2019
 *      Author: bonnee
 */

#ifndef PACK_H_
#define PACK_H_

#include <inttypes.h>
#include "ltc6804.h"
#include "chimera_config.h"

/**
 * @defgroup
 */
typedef enum
{
	PACK_INIT,
	PACK_OK,
	PACK_COMMUNICATION_ERROR,	// Refers to PEC error
	PACK_WARNING				// One or more cells need attention
} PACK_STATE_T;

/** Battery pack basic info */
typedef struct
{
	uint32_t voltage; /**< the total pack voltage */
	uint16_t max_voltage;
	uint16_t min_voltage;

	uint16_t avg_temperature; /**< the average temp of all cells of the pack */
	uint16_t max_temperature; /**< the max registered cell temperature */
	uint16_t min_temperature; /**< the min registered cell temperature */

	int16_t current;	/*!< Current exiting the pack. Expressed in Ampere * 10 */
	//int32_t current; /**< the current flowing in or out the pack */
	//PACK_STATE_T state; /**< general state of the pack */
} PACK_T;

/**
 * Defines the cell distribution inside the rdcv groups.
 * 0: cell not present
 * 1: cell present
 */
static const uint8_t cell_distribution[LTC6804_REG_COUNT * LTC6804_REG_CELL_COUNT] =
{
		1, 1, 1,	// GROUP A
		1, 1, 0,	// GROUP B
		1, 1, 1,	// GROUP C
		1, 0, 0		// GROUP D
};

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack);
void pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack);
void pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack);
void pack_update_current(PACK_T *pack);
void pack_update_status(PACK_T *pack);
LTC6804_T* pack_get_configs();
CELL_T* pack_get_cells();

#endif /* PACK_H_ */
