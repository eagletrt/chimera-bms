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
#include "stm32f3xx_hal.h"

uint32_t adcCurrent[512];
int32_t instCurrent;
int32_t current;
int32_t current_s;

LTC6804_T ltc[LTC6804_COUNT];

void pack_init(ADC_HandleTypeDef *adc, PACK_T *pack)
{
	// Start current measuring
	HAL_ADC_Start_DMA(adc, adcCurrent, 512);

	pack->state = PACK_INIT;
	pack->voltage = 0;
	pack->temperature = 0;
	pack->max_temperature = 0;
	pack->current = 0;

	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		ltc6804_init(&ltc[i], (uint8_t) 8 * i);
	}
}

PACK_STATE_T pack_update_voltages(SPI_HandleTypeDef *spi, PACK_T *pack)
{
	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		switch (ltc6804_read_voltages(spi, &ltc[i]))
		{
		case LTC6804_STATUS_OK:
			pack->state = PACK_OK;
			break;
		case LTC6804_STATUS_NONE: // ????
			pack->state=PACK_INIT;
			break;
		case LTC6804_STATUS_CELL_ERROR:
			pack->state = PACK_WARNING;
			break;
		case LTC6804_STATUS_PEC_ERROR:
			pack->state = PACK_COMMUNICATION_ERROR;
			break;
		}
	}
	return pack->state;
}

PACK_STATE_T pack_update_temperatures(SPI_HandleTypeDef *spi, PACK_T *pack)
{
	pack->state = PACK_OK;

	uint8_t parity;
	uint8_t ltc_count;
	for (parity = 0; parity < 2; parity++)
	{
		for (ltc_count = 0; ltc_count < LTC6804_COUNT; ltc_count++)
		{
			switch (ltc6804_read_temperatures(spi, parity, &ltc[ltc_count]))
			{
			case LTC6804_STATUS_OK:
				break;
			case LTC6804_STATUS_NONE:	// ?????
				pack->state = PACK_INIT;
				break;
			case LTC6804_STATUS_CELL_ERROR:
				pack->state = PACK_WARNING;
				break;
			case LTC6804_STATUS_PEC_ERROR:
				pack->state = PACK_COMMUNICATION_ERROR;
				break;
			}
		}
	}

	return pack->state;
}

void pack_update_current(PACK_T *pack)
{
	//Current
	instCurrent = 0;
	for (int i = 0; i < 512; i++)
		instCurrent += adcCurrent[i];

	instCurrent = -(instCurrent / 512 - 2595) * 12.91;
	current_s = current_s + instCurrent - (current_s / 16);
	pack->current = current_s / 16;
}

/**
 * @brief		Monitors voltages and temperatures of the battery pack
 * @param		Array of cells
 * @param		Pack object
 */
void pack_update_status(PACK_T *pack)
{
	pack->temperature = 0;
	pack->voltage = 0;
	pack->max_temperature = 0;

	uint32_t sum_temp=0;
	for (int i = 0; i < LTC6804_COUNT; i++)
	{
		sum_temp += ltc[i].avg_cell_temperature;

		pack->voltage += (uint32_t) ltc[i].cell_voltage;

		if (ltc[i].max_cell_temperature > pack->max_temperature)
		{
			pack->max_temperature = ltc[i].max_cell_temperature;
		}
	}
	pack->temperature = sum_temp / LTC6804_COUNT;

	if (pack->voltage > PACK_MAX_VOLTAGE
			|| pack->temperature > PACK_MAX_TEMPERATURE)
	{
		pack->state = PACK_WARNING;
	}
}

void pack_get_ltcs(LTC6804_T *out_ltc)
{
	out_ltc = ltc;
}

void pack_get_cells(CELL_T *cells[])
{
	uint8_t i;
	for (i = 0; i < LTC6804_COUNT; i++)
	{
		cells[i * LTC6804_CELL_COUNT] = ltc[i].cells;
	}
}
