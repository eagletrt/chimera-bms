/**
 ******************************************************************************
 * @file	ltc_68xx.c
 * @author	Dona, Riki, Gregor, Davide e Matteo
 * @brief	This file contains all the functions for the LTC68xx battery
 * 			management
 ******************************************************************************
 */

#include "ltc_68xx.h"

/**
 * @brief		This function is used to calculate the PEC value
 * @param		Length of the data array
 * @param		Array of data
 * @param		CRC table
 * @retval		16 bit unsigned integer containing the two PEC bytes
 */
uint16_t pec15(uint8_t len, uint8_t data[], const uint16_t crcTable[]) {

	uint16_t remainder, address;
	remainder = 16;					// PEC seed
	for (int i = 0; i < len; i++) {

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
uint16_t convert_voltage(uint8_t v_data[]) {

	return v_data[0] + (v_data[1] << 8);

}

/**
 * @brief		This function converts a voltage data from the zener sensor
 * 			to a temperature
 * @param		Voltage read from the LTC68xx
 * @retval 	Temperature of the cell multiplied by 100
 */
uint16_t convert_temp(uint16_t volt) {

	float voltf = volt * 0.0001;
	float temp;
	temp = -225.7 * voltf * voltf * voltf + 1310.6 * voltf * voltf
			- 2594.8 * voltf + 1767.8;
	return (uint16_t) (temp * 100);

}

/**
 * @brief		Wakes up all the devices connected to the isoSPI bus
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 			the configuration information for SPI module.
 */
void wakeup_idle(SPI_HandleTypeDef *hspi) {
	uint8_t data = 0xFF;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, &data, 1, 1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

}

/**
 * @brief		Monitors voltages and temperatures of the battery pack
 * @param		Array of cells
 * @param		Pack status
 */
void status(Cell cells[], Pack *pack) {

	pack->state = PACK_OK;

	/*
	 uint32_t sum_t = 0;
	 uint32_t pack_v_temp=0;
	 uint32_t max_t_temp=0;
	 */

	uint32_t pack_volt = 0;
	uint32_t pack_temp = 0;

	for (int i = 0; i < N_CELLS; i++) {
		cells[i].state = CELL_OK;

		/*if(cells[i].temperature > max_t_temp){
		 max_t_temp = cells[i].temperature;
		 }*/

		pack_volt += cells[i].voltage;
		pack_temp += cells[i].temperature;

		if (cells[i].voltage_faults > 10 || cells[i].temperature_faults > 10) {
			cells[i].state = CELL_DATA_NOT_UPDATED;
			pack->state = PACK_ERROR;
		}

		if (cells[i].voltage < CELL_MIN_VOLTAGE) {
			cells[i].state = CELL_UNDER_VOLTAGE;
			pack->state = PACK_ERROR;
		}

		if (cells[i].voltage > CELL_MAX_VOLTAGE) {
			cells[i].state = CELL_OVER_VOLTAGE;
			pack->state = PACK_ERROR;
		}

		if (cells[i].temperature > CELL_MAX_TEMPERATURE
				|| cells[i].temperature == 0) {
			cells[i].state = CELL_OVER_TEMPERATURE;
			pack->state = PACK_ERROR;
		}
	}

	pack->voltage = pack_volt;
	pack->temperature = (uint16_t) (pack_temp / N_CELLS);

	/*
	 *max_t=max_t_temp;
	 *pack_v=pack_v_temp;*/
}

/**
 * @brief		Reads the data form the LTC68xx and updates the cell temperatures
 * @param		Number of the LTC68xx to read the data from
 * @param		uint8_t that indicates if we are reading even or odd temperatures
 * @param		Array of temperatures
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void ltc6804_rdcv_temp(Cell cells[], SPI_HandleTypeDef *hspi) {
	wakeup_idle(hspi);

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	uint8_t ic;
	uint8_t reg;
	uint8_t parity;

	for (parity = 0; parity < 1; parity++) {
		ltc6804_command_temperatures(1, parity, hspi);	// switch between even and odd
		ltc6804_adcv(1, hspi);
		HAL_Delay(10);

		for (ic = 0; ic < TOT_IC; ic++) {
			uint8_t count = 0;
			uint8_t ic_count = ic * CELLS_PER_IC;
			cmd[0] = (uint8_t) 0x80 + 8 * ic;

			for (reg = 0; reg < N_REGISTERS; reg++) {

				cmd[1] = (uint8_t) rdcv_cmd[reg];
				cmd_pec = pec15(2, cmd, crcTable);
				cmd[2] = (uint8_t) (cmd_pec >> 8);
				cmd[3] = (uint8_t) (cmd_pec);

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
				HAL_SPI_Transmit(hspi, cmd, 4, 100);
				HAL_SPI_Receive(hspi, data, 8, 100);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

				if (pec15(6, data, crcTable) == (uint16_t) (data[6] * 256 + data[7])) {

					if (parity) {

						count++; // Skipping the first cell

						if (cell_distribution[reg * CELLS_PER_REG + 1]) {
							cells[ic_count + count + 1].temperature = convert_temp(convert_voltage(data + 2));
							cells[ic_count + count + 1].temperature_faults = 0;

							count++;
						}

						count++; // Skipping the last cell
					} else {

						if (cell_distribution[reg * CELLS_PER_REG]) {
							cells[ic_count + count].temperature = convert_temp(convert_voltage(data));
							cells[ic_count + count].temperature_faults = 0;

							count++;
						}

						count++; // Skipping the middle cell

						if (cell_distribution[reg * CELLS_PER_REG + 2]) {
							cells[ic_count + count].temperature = convert_temp(convert_voltage(data + 4));
							cells[ic_count + count].temperature_faults = 0;

							count++;
						}
					}

				} else {

					if (parity) {

						count++; // Skipping the first cell

						if (cell_distribution[reg * CELLS_PER_REG + 1]) {
							cells[ic_count + count + 1].temperature_faults++;

							count++;
						}

						count++; // Skipping the last cell
					} else {

						if (cell_distribution[reg * CELLS_PER_REG]) {
							cells[ic_count + count].temperature_faults++;

							count++;
						}

						count++; // Skipping the middle cell

						if (cell_distribution[reg * CELLS_PER_REG + 2]) {
							cells[ic_count + count].temperature_faults++;

							count++;
						}
					}
				}
			}
		}
	}

	ltc6804_command_temperatures(0, 0, hspi);	// turn off temp reading
}

/**
 * @brief		Reads the data form the LTC68xx and updates the cell voltages
 * @param		Array of cells
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void ltc6804_rdcv_voltages(Cell cells[], SPI_HandleTypeDef *hspi) {
	wakeup_idle(hspi);
	ltc6804_adcv(0, hspi);
	HAL_Delay(10);

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];


	uint8_t ic;
	uint8_t reg;
	for (ic = 0; ic < TOT_IC; ic++) {
		// if cell_distribution is 0 the count is not increased
		uint8_t count = 0;
		uint8_t ic_count = ic * CELLS_PER_IC;

		cmd[0] = (uint8_t) 0x80 + 8 * ic;

		for (reg = 0; reg < N_REGISTERS; reg++) {

			cmd[1] = (uint8_t) rdcv_cmd[reg];
			cmd_pec = pec15(2, cmd, crcTable);
			cmd[2] = (uint8_t) (cmd_pec >> 8);
			cmd[3] = (uint8_t) (cmd_pec);

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_SPI_Transmit(hspi, cmd, 4, 100);
			HAL_SPI_Receive(hspi, data, 8, 100);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

			if (pec15(6, data, crcTable) == (uint16_t) (data[6] * 256 + data[7])) {

				uint8_t cell;
				for (cell = 0; cell < CELLS_PER_REG; cell++) {

					if (cell_distribution[reg * CELLS_PER_REG + cell]) {
						cells[ic_count + count].voltage = convert_voltage(data + 2 * cell);
						cells[ic_count + count].voltage_faults = 0;
						count++;
					}
				}

			} else {

				uint8_t cell;
				for (cell = 0; cell < CELLS_PER_REG; cell++) {

					if (cell_distribution[reg * CELLS_PER_REG + cell]) {
						cells[ic_count + count].voltage_faults++;
						count++;
					}
				}
			}
		}
	}
}

/**
 * @brief		Enable or disable the temperature measurement
 * @param		1 to start temperature measurement and 0 to stop it
 * @param		uint8_t that indicates if we are reading even or odd temperatures
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void ltc6804_command_temperatures(uint8_t start, uint8_t parity, SPI_HandleTypeDef *hspi) {
	wakeup_idle(hspi);
	ltc6804_adcv(1, hspi);

	uint8_t cmd[4];
	uint8_t cfng[8];
	uint16_t cmd_pec;
	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd_pec = pec15(2, cmd, crcTable);
	cmd[2] = (uint8_t) (cmd_pec >> 8);
	cmd[3] = (uint8_t) (cmd_pec);
	cfng[0] = 0x00;
	cfng[1] = 0x00;
	cfng[2] = 0x00;
	cfng[3] = 0x00;
	if (start == 1) {
		if (parity == 0) {

			cfng[4] = 0x4A;
			cfng[5] = 0x01;

		} else {

			cfng[4] = 0x95;
			cfng[5] = 0x02;

		}

	} else {

		cfng[4] = 0x00;
		cfng[5] = 0x00;

	}
	cmd_pec = pec15(6, cfng, crcTable);
	cfng[6] = (uint8_t) (cmd_pec >> 8);
	cfng[7] = (uint8_t) (cmd_pec);

	wakeup_idle(hspi);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 10);
	HAL_SPI_Transmit(hspi, cfng, 8, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

}

/**
 * @brief		Starts the LTC68xx ADC voltage conversion
 * @param		DCP: 0 to read voltages and 1 to read temperatures
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void ltc6804_adcv(uint8_t DCP, SPI_HandleTypeDef *hspi) {

	uint8_t cmd[4];
	uint16_t cmd_pec;
	cmd[0] = (uint8_t) 0x03;
	cmd[1] = (uint8_t) 0x60 + DCP * 16;
	cmd_pec = pec15(2, cmd, crcTable);
	cmd[2] = (uint8_t) (cmd_pec >> 8);
	cmd[3] = (uint8_t) (cmd_pec);

	wakeup_idle(hspi);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

}

