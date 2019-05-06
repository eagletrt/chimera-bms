/**
 * @file	pack.c
 * @brief	This file contains the functions to manage the battery pack
 *
 * @date	Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "pack.h"

#include <stdlib.h>
#include <math.h>
#include <stm32f3xx_hal.h>

uint32_t adcCurrent[512];
int32_t instCurrent;
int32_t current;
int32_t current_s;

LTC6804_T ltc[LTC6804_COUNT];

/**
 * @brief	Initializes the pack
 *
 * @param	adc		ADC used to measure the current
 * @param	pack	Pack struct to initialize
 */
void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack)
{
	// Start current measuring
	HAL_ADC_Start_DMA(adc, adcCurrent, 512);

	pack->total_voltage = 0;
	pack->max_voltage = 0;
	pack->min_voltage = 0;
	pack->avg_temperature = 0;
	pack->max_temperature = 0;
	pack->min_temperature = 0;

	pack->current.value = 0;
	error_init(&pack->current.error);

	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		ltc[i].address = (uint8_t) 8 * i;
		ltc[i].cell_distribution = cell_distribution;// cell_distribution is not duplicated through the array of ltcs.

		error_init(&ltc[i].error);
	}

	for (i = 0; i < PACK_MODULE_COUNT; i++)
	{
		pack->voltages[i].value = 0;
		error_init(&(pack->voltages[i].error));

		// Split this cycle if temperatures has a different size than voltage

		pack->temperatures[i].value = 0;
		error_init(&pack->temperatures[i].error);
	}
}

/**
 * @brief	Polls all the LTCs for voltages
 *
 * @param	spi			isoSPI configuration struct
 * @param	voltages	The array of voltages
 * @param	error		Error return value
 */
void pack_update_voltages(SPI_HandleTypeDef *spi, ER_UINT16_T *voltages,
		ERROR_T *error)
{
	uint16_t i;
	for (i = 0; i < LTC6804_COUNT || !error; i++)
	{
		ltc6804_read_voltages(spi, &ltc[i], &voltages[i * LTC6804_CELL_COUNT],
				error);
		ER_CHK(error);
	}

	End: ;
}

/**
 * @brief	Polls all the LTCs for temperatures
 *
 * @param	spi				isoSPI configuration struct
 * @param	temperatures	The array of temperatures
 * @param	error			Error return value
 */
void pack_update_temperatures(SPI_HandleTypeDef *spi, ER_UINT16_T *temperatures,
		ERROR_T *error)
{
	uint16_t i;
	for (i = 0; i < LTC6804_COUNT || !error; i++)
	{
		ltc6804_read_temperatures(spi, &ltc[i], &temperatures[i * LTC6804_CELL_COUNT], error);
		ER_CHK(error);
	}

	End: ;
}

/**
 * @brief	Updates the current draw.
 *
 * @param	pack	The struct to save the data to
 * @param	error	The error return value
 */
void pack_update_current(PACK_T *pack, ERROR_T *error)
{
	instCurrent = 0;
	for (int i = 0; i < 512; i++)
		instCurrent += adcCurrent[i];
	instCurrent /= 512;

	float tmp_cur = (((float) instCurrent * 3.3) / 4096 - 2.048);
	pack->current.value = round((tmp_cur * 200 / 1.25) * 10);

	if (pack->current.value > PACK_MAX_CURRENT)
	{
		error_set(ERROR_OVER_CURRENT, &pack->current.error, HAL_GetTick());
	}
	else
	{
		error_unset(ERROR_OVER_CURRENT, &pack->current.error);
	}

	error_check_fatal(&pack->current.error, HAL_GetTick(), error);
	ER_CHK(error);

	End: ;
}

/**
 * @brief	Updates the pack's stats
 *
 * @param	pack	The struct to save the data to
 */
void pack_update_status(PACK_T *pack)
{
	uint32_t voltage = 0;
	uint16_t max_voltage = 0;
	uint16_t min_voltage = pack->voltages[0].value;

	uint32_t avg_temperature = 0;
	uint16_t max_temperature = 0;
	uint16_t min_temperature = pack->temperatures[0].value;

	for (int i = 0; i < PACK_MODULE_COUNT; i++)
	{
		avg_temperature += (uint32_t) pack->temperatures[i].value;

		voltage += (uint32_t) pack->voltages[i].value;

		max_voltage = fmax(max_voltage, (uint16_t) pack->voltages[i].value);
		min_voltage = fmin(min_voltage, (uint16_t) pack->voltages[i].value);

		max_temperature = fmax(max_temperature, pack->temperatures[i].value);
		min_temperature = fmin(min_temperature, pack->temperatures[i].value);
	}

	pack->total_voltage = voltage;
	pack->max_voltage = max_voltage;
	pack->min_voltage = min_voltage;

	pack->avg_temperature = (uint16_t) (avg_temperature / PACK_MODULE_COUNT);
	pack->max_temperature = max_temperature;
	pack->min_temperature = min_temperature;
}
