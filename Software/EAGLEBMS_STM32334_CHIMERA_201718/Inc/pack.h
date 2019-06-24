/**
 * @file		pack.h
 * @brief		This file contains the functions to manage the battery pack
 *
 * @date		Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef PACK_H_
#define PACK_H_

#include "chimera_config.h"
#include "error.h"
#include "ltc6804.h"
#include <inttypes.h>
#include <stdbool.h>

/** @brief Battery pack basic info */
typedef struct
{
	ER_UINT16_T voltages[PACK_MODULE_COUNT]; /*!< Voltage of the single cells */
	ER_UINT16_T temperatures[PACK_MODULE_COUNT];

	uint32_t total_voltage; /*!< Total pack voltage */
	uint16_t max_voltage;   /*!< Maximum cell voltage */
	uint16_t min_voltage;   /*!< Minimum cell voltage */

	uint16_t avg_temperature; /*!< Average temperature of all the cells */
	uint16_t max_temperature; /*!< Highest registered cell temperature */
	uint16_t min_temperature; /*!< Smallest cell temperature */

	ER_INT32_T current; /*!< Instant current draw. Expressed in Ampere * 10 */
} PACK_T;

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack);
uint8_t pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack,
							 ERROR_T *error);
uint8_t pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack,
								 ERROR_T *error);
void pack_update_current(ER_INT32_T *current, ERROR_T *error);
void pack_update_voltage_stats(PACK_T *pack);
void pack_update_temperature_stats(PACK_T *pack);

#endif /* PACK_H_ */
