/*
 * pack.c
 *
 *  Created on: Apr 11, 2019
 *      Author: bonnee
 */

#include "pack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stm32f3xx_hal.h"

uint32_t adcCurrent[512];
int32_t instCurrent;
int32_t current;
int32_t current_s;

LTC6804_T ltc[LTC6804_COUNT];
CELL_T cells[PACK_CELL_COUNT];

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack)
{
	// Start current measuring
	HAL_ADC_Start_DMA(adc, adcCurrent, 512);

	pack->voltage = 0;
	pack->max_voltage = 0;
	pack->min_voltage = 0;
	pack->avg_temperature = 0;
	pack->max_temperature = 0;
	pack->min_temperature = 0;
	pack->current = 0;

	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		ltc[i].address = (uint8_t) 8 * i;
		ltc[i].cell_distribution = cell_distribution;	// cell_distribution is not duplicated through the array.
	}

	for (i = 0; i < PACK_CELL_COUNT; i++)
	{
		cells_init(&cells[i]);
	}
}

void pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack)
{
	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		ltc6804_read_voltages(spi, &ltc[i], &cells [ i * LTC6804_CELL_COUNT]);
	}
}

void pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack)
{
	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		ltc6804_read_temperatures(spi, &ltc[i], &cells [ i * LTC6804_CELL_COUNT]);
	}
}

void pack_update_current(PACK_T *pack)
{
	//Current
	instCurrent = 0;
	for (int i = 0; i < 512; i++)
		instCurrent += adcCurrent[i];
	instCurrent /= 512;

	float tmp_cur = (((float) instCurrent * 3.3) / 4096 - 2.048);
	pack->current = round((tmp_cur * 200 / 1.25) * 10);
}

/**
 * @brief		Monitors voltages and temperatures of the battery pack
 * @param		Array of cells
 * @param		Pack object
 */
void pack_update_status(PACK_T *pack)
{
	uint32_t voltage = 0;
	uint16_t max_voltage = 0;
	uint16_t min_voltage = cells[0].voltage;

	uint32_t avg_temperature = 0;
	uint16_t max_temperature = 0;
	uint16_t min_temperature = cells[0].temperature;

	for (int i = 0; i < PACK_CELL_COUNT; i++)
	{
		avg_temperature += cells[i].temperature;

		voltage += (uint32_t) cells[i].voltage;

		max_voltage = fmax(max_voltage, cells[i].voltage);
		min_voltage = fmin(min_voltage, cells[i].voltage);

		max_temperature = fmax(max_temperature, cells[i].temperature);
		min_temperature = fmin(min_temperature, cells[i].temperature);
	}

	pack->voltage=voltage;
	pack->max_voltage=max_voltage;
	pack->min_voltage=min_voltage;

	pack->avg_temperature = (uint16_t) (avg_temperature / PACK_CELL_COUNT);
	pack->max_temperature=max_temperature;
	pack->min_temperature=min_temperature;
}

LTC6804_T* pack_get_configs()
{
	return ltc;
}

CELL_T* pack_get_cells()
{
	return cells;
}
