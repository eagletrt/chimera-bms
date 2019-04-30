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
	uint16_t temperature; /**< the average temp of all cells of the pack */
	uint16_t max_temperature; /**< the max registered cell temperature */

	int32_t current; /**< the current flowing in or out the pack */
	PACK_STATE_T state; /**< general state of the pack */
} PACK_T;

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack);
PACK_STATE_T pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack);
PACK_STATE_T pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack);
void pack_update_current(PACK_T *pack);
void pack_update_status(PACK_T *pack);
void pack_get_cells(CELL_T **cells);
void pack_get_ltcs(LTC6804_T *ltc);

#endif /* PACK_H_ */
