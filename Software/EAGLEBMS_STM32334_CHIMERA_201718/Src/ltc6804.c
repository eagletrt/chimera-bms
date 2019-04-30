/*
 * ltc6804.c
 *
 *  Created on: Apr 11, 2019
 *      Author: bonnee
 */

#include "ltc6804.h"

#include <stdlib.h>
#include <string.h>

void ltc6804_init(LTC6804_T *ltc, uint8_t address)
{
	ltc->status = LTC6804_STATUS_NONE;
	ltc->address = address;

	ltc->cell_voltage = 0;
	ltc->avg_cell_temperature = 0;
	ltc->max_cell_temperature = 0;

	uint8_t i;
	for (i = 0; i < LTC6804_CELL_COUNT; i++)
	{
		ltc->cells[i].voltage = 0;
		ltc->cells[i].temperature = 0;
		ltc->cells[i].voltage_faults = 0;
		ltc->cells[i].temperature_faults = 0;
		ltc->cells[i].state = CELL_NONE;
	}
}

/**
 * @brief		Polls all the registers of the LTC6804 and updates the cell array
 * @param 		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 * @param		Pointer to the LTC6804_T.
 */
LTC6804_STATUS_T ltc6804_read_voltages(SPI_HandleTypeDef *hspi, LTC6804_T *ltc)
{
	ltc->status = LTC6804_STATUS_OK;

	_ltc6804_adcv(hspi, 0);

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	cmd[0] = (uint8_t) 0x80 + ltc->address;

	uint8_t count = 0; // cells[] index
	uint8_t reg; // Counts the register
	for (reg = 0; reg < LTC6804_REG_COUNT; reg++)
	{
		cmd[1] = (uint8_t) rdcv_cmd[reg];
		cmd_pec = _pec15(2, cmd);
		cmd[2] = (uint8_t) (cmd_pec >> 8);
		cmd[3] = (uint8_t) (cmd_pec);

		_wakeup_idle(hspi);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_SPI_Transmit(hspi, cmd, 4, 100);

		HAL_SPI_Receive(hspi, data, 8, 100);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

		uint8_t pec = _pec15(6, data) == (uint16_t) (data[6] * 256 + data[7]);

		uint8_t cell = 0; // Counts the cell inside the register
		for (cell = 0; cell < LTC6804_REG_CELL_COUNT; cell++)
		{
			if (cell_distribution[reg * LTC6804_REG_CELL_COUNT + cell])
			{
				// If cell is present
				if (pec)
				{
					ltc->cells[count].voltage = _convert_voltage(&data[2 * cell]);
					ltc->cells[count].voltage_faults = 0;

					count++;
				}
				else
				{
					ltc->cells[count].voltage_faults++;
					ltc->status = LTC6804_STATUS_PEC_ERROR;
				}

				if (ltc6804_update_state(&ltc->cells[count]) != CELL_OK)
				{
					// Report an error if cell is not ok
					ltc->status = LTC6804_STATUS_CELL_ERROR;
				}
			}
		}
	}

	ltc6804_compute_total_values(ltc);
	return ltc->status;
}

void ltc6804_compute_total_values(LTC6804_T *ltc)
{
	ltc->cell_voltage = 0;
	ltc->avg_cell_temperature = 0;
	ltc->max_cell_temperature = 0;

	uint32_t tmp_temperature = 0;
	uint8_t i;
	for (i = 0; i < LTC6804_CELL_COUNT; i++)
	{
		ltc->cell_voltage += ltc->cells[i].voltage;
		tmp_temperature += ltc->cells[i].temperature;

		if (ltc->cells[i].temperature > ltc->max_cell_temperature)
		{
			ltc->max_cell_temperature = ltc->cells[i].temperature;
		}
	}

	ltc->avg_cell_temperature = (uint16_t) tmp_temperature / LTC6804_CELL_COUNT;
}

/**
 * @brief		Starts the LTC6804 ADC voltage conversion
 * @param		DCP: 0 to read voltages and 1 to read temperatures
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void _ltc6804_adcv(SPI_HandleTypeDef *hspi, uint8_t DCP)
{
	uint8_t cmd[4];
	uint16_t cmd_pec;
	cmd[0] = (uint8_t) 0x03;
	cmd[1] = (uint8_t) 0x60 + DCP * 16;
	cmd_pec = _pec15(2, cmd);
	cmd[2] = (uint8_t) (cmd_pec >> 8);
	cmd[3] = (uint8_t) (cmd_pec);

	_wakeup_idle(hspi);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 100);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

/**
 * @brief		Enable or disable the temperature measurement through balancing
 * @param		1 to start temperature measurement and 0 to stop it
 * @param		uint8_t that indicates if we are reading even or odd temperatures
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void _ltc6804_command_temperatures(SPI_HandleTypeDef *hspi, uint8_t start,
		uint8_t parity)
{
	uint8_t cmd[4];	// WRCFG command
	uint8_t cfng[8];// Set which cells to balance. It is necessary to not read adiacent cells at the same time, that's the purpose of the parity variable
	uint16_t cmd_pec;
	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd_pec = _pec15(2, cmd);
	cmd[2] = (uint8_t) (cmd_pec >> 8);
	cmd[3] = (uint8_t) (cmd_pec);

	cfng[0] = 0x00;
	cfng[1] = 0x00;
	cfng[2] = 0x00;
	cfng[3] = 0x00;

	if (start)
	{
		if (parity)
		{
			// Command to balance cells (in order) 8,5,3,1 and 10
			cfng[4] = 0b10010101;
			cfng[5] = 0b00000010;
		}
		else
		{
			// Command to balance cells (in order) 7,4,2 and 9
			cfng[4] = 0b01001010;
			cfng[5] = 0b00000001; // First 4 zeros are for DCT0 and should remain 0
		}
	}
	else
	{
		cfng[4] = 0x00;
		cfng[5] = 0x00;
	}
	cmd_pec = _pec15(6, cfng);
	cfng[6] = (uint8_t) (cmd_pec >> 8);
	cfng[7] = (uint8_t) (cmd_pec);

	_wakeup_idle(hspi);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

	HAL_SPI_Transmit(hspi, cmd, 4, 10);
	HAL_Delay(1);
	HAL_SPI_Transmit(hspi, cfng, 8, 10);
	HAL_Delay(1);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

	_ltc6804_adcv(hspi, start);
}

LTC6804_STATUS_T ltc6804_read_temperatures(SPI_HandleTypeDef *hspi,
		uint8_t parity, LTC6804_T *ltc)
{
	ltc->status = LTC6804_STATUS_OK;

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	_ltc6804_command_temperatures(hspi, 1, parity); // switch between even and odd
	//HAL_Delay(5);
	_ltc6804_adcv(hspi, 1);


	cmd[0] = (uint8_t) 0x80 + ltc->address;

	uint8_t reg;
	uint8_t count = 0;
	for (reg = 0; reg < LTC6804_REG_COUNT; reg++)
	{
		cmd[1] = (uint8_t) rdcv_cmd[reg];
		cmd_pec = _pec15(2, cmd);
		cmd[2] = (uint8_t) (cmd_pec >> 8);
		cmd[3] = (uint8_t) (cmd_pec);

		_wakeup_idle(hspi);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_SPI_Transmit(hspi, cmd, 4, 100);

		HAL_SPI_Receive(hspi, data, 8, 100);
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

		uint8_t pec = _pec15(6, data) == (uint16_t) (data[6] * 256 + data[7]);

		uint8_t cell = 0; // Counts the cell inside the register
		for (cell = 0; cell < LTC6804_REG_CELL_COUNT; cell++)
		{
			uint8_t reg_cell = reg * LTC6804_REG_CELL_COUNT;

			if (parity)		// If we're reading even cells
			{
				if (count % 2 == 0)		// If the cell is even
				{
					if (cell_distribution[reg_cell + cell])		// If the cell is present
					{
						if (pec)
						{
							ltc->cells[count].temperature = _convert_temp(_convert_voltage(&data[2 * cell]));
							ltc->cells[count].temperature_faults = 0;
						}
						else
						{
							ltc->cells[count].temperature_faults++;
							ltc->status = LTC6804_STATUS_PEC_ERROR;
						}

						ltc6804_update_state(&(ltc->cells[count]));
						count++;
					}
				}
				else{
					if (cell_distribution[reg_cell + cell])
					{
						count++;
					}
				}
			}
			else
			{
				if (count % 2 != 0)		// If the cell is odd
				{
					if (cell_distribution[reg_cell + cell])
					{
						if (pec)
						{
							ltc->cells[count].temperature = _convert_temp(_convert_voltage(&data[2 * cell]));
							ltc->cells[count].temperature_faults = 0;

						}
						else
						{
							ltc->cells[count].temperature_faults++;
							ltc->status = LTC6804_STATUS_PEC_ERROR;
						}

						ltc6804_update_state(&(ltc->cells[count]));
						count++;
					}
				}
				else
				{

					// Skip middle cell
					if (cell_distribution[reg_cell + cell])
					{
						count++;
					}
				}
			}
		}

	}

	_ltc6804_command_temperatures(hspi, 0, 0);	// turn off temp reading

	ltc6804_compute_total_values(ltc);
	return ltc->status;
}

CELL_STATE_T ltc6804_update_state(CELL_T *cell)
{
	if (!cell->voltage_faults && !cell->temperature_faults)
	{
		cell->state = CELL_OK;
	}

	if (cell->voltage < CELL_MIN_VOLTAGE && !cell->voltage_faults)
	{
		cell->state = CELL_UNDER_VOLTAGE;
	}

	if (cell->voltage > CELL_MAX_VOLTAGE && !cell->voltage_faults)
	{
		cell->state = CELL_OVER_VOLTAGE;
	}

	if (cell->temperature >= CELL_MAX_TEMPERATURE && !cell->temperature_faults)
	{
		cell->state = CELL_OVER_TEMPERATURE;
	}

	if (cell->temperature <= CELL_MIN_TEMPERATURE && !cell->temperature_faults)
	{
		cell->state = CELL_UNDER_TEMPERATURE;
	}

	/*if (cell->voltage_faults > 0 || cell->temperature_faults > 0)
	{
		cell->state = CELL_DATA_NOT_UPDATED;
	}*/

	return cell->state;
}

/**
 * @brief		Wakes up all the devices connected to the isoSPI bus
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void _wakeup_idle(SPI_HandleTypeDef *hspi)
{
	uint8_t data = 0xFF;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, &data, 1, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

/**
 * @brief		This function is used to calculate the PEC value
 * @param		Length of the data array
 * @param		Array of data
 * @param		CRC table
 * @retval		16 bit unsigned integer containing the two PEC bytes
 */
uint16_t _pec15(uint8_t len, uint8_t data[])
{
	uint16_t remainder, address;
	remainder = 16;					// PEC seed
	for (int i = 0; i < len; i++)
	{

		address = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
		remainder = (remainder << 8) ^ crcTable[address];

	}
	return (remainder * 2);	//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}

/**
 * @brief		This function is used to convert the 2 byte raw data from the
 * 			LTC68xx to a 16 bit unsigned integer
 * @param 		Raw data bytes
 * @retval		Voltage read from the LTC68xx
 */
uint16_t _convert_voltage(uint8_t v_data[])
{
	return v_data[0] + (v_data[1] << 8);
}

/**
 * @brief		This function converts a voltage data from the zener sensor
 * 			to a temperature
 * @param		Voltage read from the LTC68xx
 * @retval 	Temperature of the cell multiplied by 100
 */
uint16_t _convert_temp(uint16_t volt)
{
	float voltf = volt * 0.0001;
	float temp;
	temp = -225.7 * voltf * voltf * voltf + 1310.6 * voltf * voltf
			- 2594.8 * voltf + 1767.8;
	return (uint16_t) (temp * 100);
}
