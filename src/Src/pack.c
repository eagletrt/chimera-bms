/**
 * @file		pack.c
 * @brief		This file contains the functions to manage the battery pack
 *
 * @date		Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "pack.h"

#include <math.h>
#include <stdbool.h>
#include <stm32f3xx_hal.h>
#include <string.h>

#define CURRENT_ARRAY_LENGTH 512
#define PACK_DROP_DELTA 2000  // 0.2V

uint32_t adc_current[CURRENT_ARRAY_LENGTH];

LTC6804_T ltc[LTC6804_COUNT];

/**
 * @brief	Initializes the pack
 *
 * @param	adc		The configuration structure for the ADC
 * @param	pack	The PACK_T struct to initialize
 */
void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack) {
	// Start current measuring
	HAL_ADC_Start_DMA(adc, adc_current, CURRENT_ARRAY_LENGTH);

	pack->total_voltage = 0;
	pack->max_voltage = 0;
	pack->min_voltage = 0;
	pack->avg_temperature = 0;
	pack->max_temperature = 0;
	pack->min_temperature = 0;

	pack->current.value = 0;
	error_init(&pack->current.error);

	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++) {
		ltc[i].address = (uint8_t)8 * i;
		// cell_distribution is not duplicated through the array of ltcs
		ltc[i].cell_distribution = cell_distribution;

		error_init(&ltc[i].error);
	}

	for (i = 0; i < PACK_MODULE_COUNT; i++) {
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
 * @param		pack			The PACK_T struct to update
 * @param		warning		The warning return value
 * @param		error			The error return value
 *
 * @returns	The index of the last updated cell
 */
uint8_t pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack,
							 WARNING_T *warning, ERROR_T *error) {
	uint8_t ltc_i, cell;

	_ltc6804_adcv(spi, 0);

	for (ltc_i = 0; ltc_i < LTC6804_COUNT || !error; ltc_i++) {
		cell = ltc6804_read_voltages(
			spi, &ltc[ltc_i], &pack->voltages[ltc_i * LTC6804_CELL_COUNT],
			warning, error);
		ER_CHK(error);
	}

End:;
	pack_update_voltage_stats(pack);

	if (error == ERROR_LTC6804_PEC_ERROR) {
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
 * @param		spi			The SPI configuration structure
 * @param		pack		The PACK_T struct to update
 * @param		error		The error return value
 *
 * @returns	The index of the last updated cell
 */
uint8_t pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack,
								 ERROR_T *error) {
	static uint8_t ltc_index = 0;
	static bool even = 0;
	uint8_t cell_index = 0;

	ltc6804_configure_temperature(spi, true, even);

	// Read 2 LTCs at a time. Roll back to 0 if limit exceeded
	uint8_t tmp = (ltc_index + 2) % LTC6804_COUNT;
	while (ltc_index != tmp) {
		cell_index = ltc6804_read_temperatures(
			spi, &ltc[ltc_index], even,
			&pack->temperatures[ltc_index * LTC6804_CELL_COUNT], error);

		ER_CHK(error);

		ltc_index = (ltc_index + 1) % LTC6804_COUNT;
		if (ltc_index == 0) {
			even = !even;
		}
	}

	ltc6804_configure_temperature(spi, false, even);

	pack_update_temperature_stats(pack);

End:;

	if (*error == ERROR_LTC6804_PEC_ERROR) {
		return ltc_index;
	}
	return cell_index;
}

/**
 * @brief		Calculates the current exiting/entering the pack
 *
 * @param		current		The current value to update
 * @param		error		The error return value
 */
void pack_update_current(ER_INT16_T *current, ERROR_T *error) {
	int32_t tmp = 0;
	uint16_t i;
	for (i = 0; i < CURRENT_ARRAY_LENGTH; i++) {
		tmp += adc_current[i];
	}
	tmp /= CURRENT_ARRAY_LENGTH;

	// We calculate the input voltage
	float in_volt = (((float)tmp * 3.3) / 4096);

	// Check the LEM HTFS 200-P datasheet for the correct formula
	current->value = (int16_t)(-round((((in_volt - 2.048) * 200 / 1.25)) * 10));
	current->value += 100;

	if (current->value > PACK_MAX_CURRENT) {
		error_set(ERROR_OVER_CURRENT, &current->error, HAL_GetTick());
	} else {
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
void pack_update_voltage_stats(PACK_T *pack) {
	uint32_t tot_voltage = pack->voltages[0].value;
	uint16_t max_voltage = pack->voltages[0].value;
	uint16_t min_voltage = pack->voltages[0].value;

	uint8_t i;
	for (i = 1; i < PACK_MODULE_COUNT; i++) {
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
void pack_update_temperature_stats(PACK_T *pack) {
	uint32_t avg_temperature = 0;
	uint16_t max_temperature = 0;
	uint16_t min_temperature = 0xFFFF;

	uint8_t temp_count = 0;
	for (int i = 0; i < PACK_MODULE_COUNT; i++) {
		if (pack->temperatures[i].value > 0) {
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

uint8_t pack_check_errors(PACK_T *pack, ERROR_T *error) {
	*error = ERROR_OK;
	WARNING_T warning;

	uint8_t i;
	for (i = 0; i < PACK_MODULE_COUNT; i++) {
		ltc6804_check_voltage(&pack->voltages[i], &warning, error);
		ER_CHK(error);
		ltc6804_check_temperature(&pack->temperatures[i], error);
		ER_CHK(error);
	}

End:
	return i;
}

/**
 * @brief		Checks if there are cells that have a higher than average
 * 					voltage drop under load
 *
 * @details	When called during an idle period (current draw under a certain
 * 					amount), this function stores the total voltage as a
 * 					reference "idle voltage". When under load, this function
 * 					compares the total voltage to the idling voltage and if
 * 					it sees a drop, checks for every cell whether the drop in
 * 					voltage is higher than average.
 *
 * @param		pack		The PACK_T to check
 * @param		cells		The array of indexes that are found to be
 * dropping too much
 * @returns	The number of cells that triggered the warning
 */
uint8_t pack_check_voltage_drops(PACK_T *pack,
								 uint8_t cells[PACK_MODULE_COUNT]) {
	static uint16_t idle_voltage = 0;
	static uint16_t idle_volts[PACK_MODULE_COUNT] = {0};

	size_t cell_index = 0;

	if (pack->current.value >= -10 && pack->current.value < 100)  // < 10A
	{  // Pack idle state
		idle_voltage = pack->total_voltage;

		uint8_t i;
		for (i = 0; i < PACK_MODULE_COUNT; i++) {
			idle_volts[i] = pack->voltages[i].value;
		}
	}

	if (pack->current.value > 300 && idle_voltage > 0)	// > 30A
	{													// Pack load state
		if (pack->total_voltage <
			idle_voltage - PACK_MODULE_COUNT * PACK_DROP_DELTA) {
			uint8_t i;
			for (i = 0; i < PACK_MODULE_COUNT; i++) {
				if (pack->voltages[i].value <
					idle_volts[i] - (PACK_DROP_DELTA +
									 1000U)) {	// If the cell dropped >0.1V
												// more than the average

					cells[cell_index++] = i;
				}
			}
		}
	}

	return cell_index;
}
