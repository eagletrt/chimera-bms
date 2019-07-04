/**
 * @file		pack.c
 * @brief		This file contains the functions to manage the battery pack
 *
 * @date		Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "pack.h"
#include <math.h>
#include <stdlib.h>
#include <stm32f3xx_hal.h>

/**
 * @details	Defines the cell distribution inside the rdcv groups:
 * 					0: cell not present
 * 					1: cell present
 */
static const bool
	cell_distribution[LTC6804_REG_COUNT * LTC6804_REG_CELL_COUNT] = {
		1, 1, 1, // GROUP A
		1, 1, 0, // GROUP B
		1, 1, 1, // GROUP C
		1, 0, 0  // GROUP D
};

uint32_t adcCurrent[512];
int32_t instCurrent;
int32_t current;
int32_t current_s;

LTC6804_T ltc[LTC6804_COUNT];

/**
 * @brief	Initializes the pack
 *
 * @param	adc		The configuration structure for the ADC
 * @param	pack	The Pack struct to initialize
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
		ltc[i].address = (uint8_t)8 * i;
		// cell_distribution is not duplicated through the array of ltcs
		ltc[i].cell_distribution = cell_distribution;

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
 * @brief		Polls all the LTCs for voltages
 * @details	This function should take 10~11ms to fully execute
 *
 * @param		spi				The SPI configuration structure
 * @param		voltages	The array of voltages
 * @param		error			The error return value
 */
uint8_t pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack,
							 WARNING_T *warning, ERROR_T *error)
{
	uint8_t ltc_i, cell;

	_ltc6804_adcv(spi, 0);

	for (ltc_i = 0; ltc_i < LTC6804_COUNT || !error; ltc_i++)
	{
		cell = ltc6804_read_voltages(
			spi, &ltc[ltc_i], &pack->voltages[ltc_i * LTC6804_CELL_COUNT],
			warning, error);
		ER_CHK(error);
	}

End:;
	pack_update_voltage_stats(pack);

	if (error == ERROR_LTC6804_PEC_ERROR)
	{
		return ltc_i;
	}
	return cell;
}

/**
 * @brief		Polls the LTCs for temperatures
 * @details	Temperature measurements with the current hardware architecture
 * 					lasts 600ms: too slow for the BMS. To avoid stopping for
 * 					that long, everytime this function is called only two LTCs
 * 					are polled, and only even or odd cells are measured from
 * 					them. When all LTCs have been polled, we start from the
 * 					beginning and we flip the odd/even bit to read the remaining
 * 					cells. This decreases the update frequency on a single cell,
 * 					but greatly improves (~10x) the blocking time of temperature
 * 					measurements. The time to update a single cell can be
 * 					calculated as following: 2*CELL_COUNT*(52ms+CALL_INTERVAL)
 *
 * @param	spi						The SPI configuration structure
 * @param	temperatures	The array of temperatures
 * @param	error					The error return value
 */
uint8_t pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack,
								 ERROR_T *error)
{
	static uint8_t ltc_index = 0;
	static bool even = 0;
	uint8_t cell_index = 0;

	ltc6804_configure_temperature(spi, true, even);

	// Read 2 LTCs at a time. Roll back to 0 if limit exceeded
	uint8_t tmp = (ltc_index + 2) % LTC6804_COUNT;
	while (ltc_index != tmp)
	{
		cell_index = ltc6804_read_temperatures(
			spi, &ltc[ltc_index], even,
			&pack->temperatures[ltc_index * LTC6804_CELL_COUNT], error);
		ER_CHK(error);

		ltc_index = (ltc_index + 1) % LTC6804_COUNT;
		if (ltc_index == 0)
		{
			even = !even;
		}
	}

	ltc6804_configure_temperature(spi, false, even);

	pack_update_temperature_stats(pack);

End:;

	if (*error == ERROR_LTC6804_PEC_ERROR)
	{
		return ltc_index;
	}
	return cell_index;
}

/**
 * @brief	Updates the current draw
 *
 * @param	current	The current value
 * @param	error		The error return value
 */
void pack_update_current(ER_INT32_T *current, ERROR_T *error)
{
	instCurrent = 0;
	for (int i = 0; i < 512; i++)
		instCurrent += adcCurrent[i];
	instCurrent /= 512;

	float tmp_cur = (((float)instCurrent * 3.3) / 4096 - 2.048);
	current->value = round((tmp_cur * 200 / 1.25) * 10);

	if (current->value > PACK_MAX_CURRENT)
	{
		error_set(ERROR_OVER_CURRENT, &current->error, HAL_GetTick());
	}
	else
	{
		error_unset(ERROR_OVER_CURRENT, &current->error);
	}

	*error = error_check_fatal(&current->error, HAL_GetTick());
	ER_CHK(error);

End:;
}

/**
 * @brief		Updates the pack's voltage stats
 * @details	It updates *_voltage variables with the data of the pack
 *
 * @param		pack	The struct to save the data to
 */
void pack_update_voltage_stats(PACK_T *pack)
{
	uint32_t tot_voltage = pack->voltages[0].value;
	uint16_t max_voltage = pack->voltages[0].value;
	uint16_t min_voltage = pack->voltages[0].value;

	uint8_t i;
	for (i = 1; i < PACK_MODULE_COUNT; i++)
	{
		tot_voltage += (uint32_t)pack->voltages[i].value;

		max_voltage = fmax(max_voltage, pack->voltages[i].value);
		min_voltage = fmin(min_voltage, pack->voltages[i].value);
	}

	pack->total_voltage = tot_voltage;
	pack->max_voltage = max_voltage;
	pack->min_voltage = min_voltage;
}

/**
 * @brief		Updates the pack's temperature stats
 * @details	It updates *_temperature variables with the data of the pack
 *
 * @param		pack	The struct to save the data to
 */
void pack_update_temperature_stats(PACK_T *pack)
{
	uint8_t temp_count = 0;

	uint32_t avg_temperature = 0;
	uint16_t max_temperature = 0;
	uint16_t min_temperature = 0xFFFF;

	for (int i = 0; i < PACK_MODULE_COUNT; i++)
	{
		if (pack->temperatures[i].value > 0)
		{
			avg_temperature += (uint32_t)pack->temperatures[i].value;

			max_temperature =
				fmax(max_temperature, pack->temperatures[i].value);
			min_temperature =
				fmin(min_temperature, pack->temperatures[i].value);
			temp_count++;
		}
	}

	pack->avg_temperature = (uint16_t)(avg_temperature / temp_count);
	pack->max_temperature = max_temperature;
	pack->min_temperature = min_temperature;
}
