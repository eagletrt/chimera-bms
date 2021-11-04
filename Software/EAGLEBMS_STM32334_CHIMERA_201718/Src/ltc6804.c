/**
 * @file		ltc6804.c
 * @brief		This file contains the functions to communicate with the LTCs
 *
 * @date		Apr 11, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "ltc6804.h"

#include "main.h"

// Set to 1 to emulate the LTC daisy chain
#define LTC6804_EMU 0

uint16_t _pec15(uint8_t len, uint8_t data[]);
uint16_t _convert_voltage(uint8_t v_data[]);
uint16_t _convert_temp(uint16_t volt);
void _wakeup_idle(SPI_HandleTypeDef *hspi, bool apply_delay);

void _ltc6804_wrcfg(SPI_HandleTypeDef *hspi, bool start, bool parity);

void ltc6804_enable_cs(SPI_HandleTypeDef *spi, GPIO_TypeDef *gpio, uint16_t pin) {
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
	while (spi->State != HAL_SPI_STATE_READY)
		;
}

void ltc6804_disable_cs(SPI_HandleTypeDef *spi, GPIO_TypeDef *gpio, uint16_t pin) {
	while (spi->State != HAL_SPI_STATE_READY)
		;
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);
}

/**
 * @brief		Polls all the registers of the LTC6804 and updates the cell
 * array
 * @details	It executes multiple rdcv requests to the LTCs and saves the values
 * 					in the voltage variable of the CELL_Ts.
 *
 * 					1     CMD0    8     CMD1      16      32
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					1 X X X X 0 0 0 0 0 0 0 X X X X  PEC
 * 					 Address |             |  Reg  |
 *
 * @param		spi		The SPI configuration structure
 * @param		ltc		The array of LTC6804 configurations
 * @param		volts	The array of voltages
 * @param		error	The error return value
 */
uint8_t ltc6804_read_voltages(SPI_HandleTypeDef *spi, LTC6804_T *ltc, ER_UINT16_T *volts, WARNING_T *warning,
							  ERROR_T *error) {
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	cmd[0] = (uint8_t)0x80 + ltc->address;

	uint8_t count = 0;	// cells[] index
	uint8_t reg;		// Counts the register

	for (reg = 0; reg < LTC6804_REG_COUNT; reg++) {
		cmd[1] = (uint8_t)rdcv_cmd[reg];
		cmd_pec = _pec15(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		_wakeup_idle(spi, false);

		ltc6804_enable_cs(spi, CS_6820_GPIO_Port, CS_6820_Pin);
		HAL_SPI_Transmit(spi, cmd, 4, 10);
		HAL_SPI_Receive(spi, data, 8, 100);
		ltc6804_disable_cs(spi, CS_6820_GPIO_Port, CS_6820_Pin);

#if LTC6804_EMU > 0
		// Writes 3.6v to each cell

		uint8_t emu_i;
		for (emu_i = 0; emu_i < LTC6804_REG_CELL_COUNT * 2; emu_i++) {
			// 36000
			data[emu_i] = 0b10100000;
			data[++emu_i] = 0b10001100;
		}
		uint16_t emu_pec = _pec15(6, data);
		data[6] = (uint8_t)(emu_pec >> 8);
		data[7] = (uint8_t)emu_pec;
#endif

		bool pec = (_pec15(6, data) == (uint16_t)(data[6] * 256 + data[7]));

		if (pec) {
			error_unset(ERROR_LTC6804_PEC_ERROR, &ltc->error);

			uint8_t cell = 0;  // Counts the cell inside the register
			for (cell = 0; cell < LTC6804_REG_CELL_COUNT; cell++) {
				// If cell is present
				if (ltc->cell_distribution[reg * LTC6804_REG_CELL_COUNT + cell]) {
					volts[count].value = _convert_voltage(&data[2 * cell]);

					ltc6804_check_voltage(&volts[count], warning, error);
					ER_CHK(error);

					count++;
				}
			}
		} else {
			error_set(ERROR_LTC6804_PEC_ERROR, &ltc->error, HAL_GetTick());
		}

		*error = error_check_fatal(&ltc->error, HAL_GetTick());
		ER_CHK(error);
	}
End:;
	return count;
}

/**
 * @brief		Starts the LTC6804 ADC voltage conversion
 * @details	According to the datasheet, this command should take 2,335µs.
 * 					ADCV Command syntax:
 *
 * 					1     CMD0    8     CMD1      16      32
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 0 1 1 0 1 1 X 0 0 0 0  PEC
 * 					 Address |    | |     |
 * 					  (BRD)      Speed   DCP
 *
 * @param		spi		The spi configuration structure
 * @param		dcp		false to read voltages; true to read temperatures
 */
void _ltc6804_adcv(SPI_HandleTypeDef *spi, bool dcp) {
	uint8_t cmd[4];
	uint16_t cmd_pec;
	cmd[0] = (uint8_t)0b00000011;
	cmd[1] = (uint8_t)0b01100000 + dcp * 0b00010000;
	cmd_pec = _pec15(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	_wakeup_idle(spi, false);
	ltc6804_enable_cs(spi, CS_6820_GPIO_Port, CS_6820_Pin);
	HAL_SPI_Transmit(spi, cmd, 4, 10);
	ltc6804_disable_cs(spi, CS_6820_GPIO_Port, CS_6820_Pin);
}

/**
 * @brief		Enable or disable the temperature measurement through balancing
 * @details	Since it's not possible to read the temperatures from adiacent
 * 					cells at the same time, We split the measurement into two
 *					times, read odd cells, and then even ones. To write
 *					configuration you have to send 2 consecutive commands:
 *
 *					WRCFG:
 * 					1     CMD0    8     CMD1      16      32
 * 					|- - - - - - -|- - - - - - - -|- ... -|
 * 					0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1  PEC
 *
 *					CFGR:
 * 					1             8               16
 * 					|- - - - - - -|- - - - - - - -|
 * 					0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 * 					17            24              32
 * 					|- - - - - - -|- - - - - - - -|
 * 					0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 *					33            40              48      64
 *					|- - - - - - -|- - - - - - - -|- ... -|
 *					1 0 0 1 0 1 0 1 0 0 0 0 0 0 1 0  PEC	<- For odd cells
 *					              or
 *					0 1 0 0 1 0 1 0 0 0 0 0 0 0 0 1  PEC	<- For even cells
 *
 * @param		hspi			The SPI configuration structure
 * @param		start_bal	whether to start temperature measurement
 * @param		even			Indicates whether we're reading odd or even
 *cells
 */
void _ltc6804_wrcfg(SPI_HandleTypeDef *hspi, bool start_bal, bool even) {
	uint8_t wrcfg[4];
	uint8_t cfgr[8];

	uint16_t cmd_pec;

	wrcfg[0] = 0x00;
	wrcfg[1] = 0x01;
	cmd_pec = _pec15(2, wrcfg);
	wrcfg[2] = (uint8_t)(cmd_pec >> 8);
	wrcfg[3] = (uint8_t)(cmd_pec);

	cfgr[0] = 0x00;
	cfgr[1] = 0x00;
	cfgr[2] = 0x00;
	cfgr[3] = 0x00;

	if (start_bal) {
		if (even) {
			// Command to balance cells (in order) 8,5,3,1 and 10
			cfgr[4] = 0b10010101;
			cfgr[5] = 0b00000010;
		} else {
			// Command to balance cells (in order) 7,4,2 and 9
			cfgr[4] = 0b01001010;
			// First 4 bits are for DCT0 and should remain 0
			cfgr[5] = 0b00000001;
		}
	} else {
		cfgr[4] = 0x00;
		cfgr[5] = 0x00;
	}
	cmd_pec = _pec15(6, cfgr);
	cfgr[6] = (uint8_t)(cmd_pec >> 8);
	cfgr[7] = (uint8_t)(cmd_pec);

	_wakeup_idle(hspi, true);

	ltc6804_enable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);

	HAL_SPI_Transmit(hspi, wrcfg, 4, 100);
	HAL_SPI_Transmit(hspi, cfgr, 8, 100);

	ltc6804_disable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);

	// TODO: remove this
	_ltc6804_adcv(hspi, start_bal);
}

void ltc6804_configure_temperature(SPI_HandleTypeDef *hspi, bool enable, bool even) {
	_ltc6804_wrcfg(hspi, enable, even);	 // switch between even and odd
	_ltc6804_adcv(hspi, enable);
}

/**
 * @brief		This function is used to fetch the temperatures.
 * @details	The workings of this function are very similar to read_voltages,
 * 					the main difference to it is the presence of the even
 * 					parameter. Refer to the ltc6804_read_voltages comment for
 * 					the actual messages.
 *
 * @param		hspi		The SPI configuration structure
 * @param		even		indicates which set of cells is currently being
 * 									balanced: false for odd and true for even
 * cells
 * @param		ltc			The array of LTC6804 configurations
 * @param		temps		The array of temperatures
 * @param		error		The error return value
 */
uint8_t ltc6804_read_temperatures(SPI_HandleTypeDef *hspi, LTC6804_T *ltc, bool even, ER_UINT16_T *temps,
								  ERROR_T *error) {
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	cmd[0] = (uint8_t)0x80 + ltc->address;

	uint8_t reg;
	uint8_t count = 0;
	for (reg = 0; reg < LTC6804_REG_COUNT; reg++) {
		cmd[1] = (uint8_t)rdcv_cmd[reg];
		cmd_pec = _pec15(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		_wakeup_idle(hspi, false);

		ltc6804_enable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);
		HAL_Delay(0);

		HAL_SPI_Transmit(hspi, cmd, 4, 10);
		HAL_SPI_Receive(hspi, data, 8, 10);

		ltc6804_disable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);

#if LTC6804_EMU > 0
		// Writes 0.9292v (18°C) to each sensor

		uint8_t emu_i;
		for (emu_i = 0; emu_i < LTC6804_REG_CELL_COUNT * 2; emu_i++) {
			// 9292
			data[emu_i] = 0b00100100;
			data[++emu_i] = 0b01001100;
		}
		uint16_t emu_pec = _pec15(6, data);
		data[6] = (uint8_t)(emu_pec >> 8);
		data[7] = (uint8_t)emu_pec;
#endif

		bool pec = (_pec15(6, data) == (uint16_t)(data[6] * 256 + data[7]));

		if (pec) {
			error_unset(ERROR_LTC6804_PEC_ERROR, &ltc->error);

			uint8_t cell = 0;  // Counts the cell inside the register
			for (cell = 0; cell < LTC6804_REG_CELL_COUNT; cell++) {
				uint8_t reg_cell = reg * LTC6804_REG_CELL_COUNT;

				// If the cell is present
				if (ltc->cell_distribution[reg_cell + cell]) {
					bool is_even = count % 2 == 0;
					// If the cell we're reading respects the even condition
					if (is_even == even) {
						uint16_t temp = _convert_temp(_convert_voltage(&data[2 * cell]));

						if (temp > 0) {
							temps[count].value = temp;

							ltc6804_check_temperature(&temps[count], error);
							ER_CHK(error);
						}
					}
					count++;
				}
			}
		} else {
			error_set(ERROR_LTC6804_PEC_ERROR, &ltc->error, HAL_GetTick());
		}
	}

	*error = error_check_fatal(&ltc->error, HAL_GetTick());
	ER_CHK(error);	// In case of error, set the error and goto label End

End:;

	return count;
}

/**
 * @brief		Checks that voltage is between its thresholds.
 *
 * @param		volts		The voltage
 * @param		error		The error return code
 */
void ltc6804_check_voltage(ER_UINT16_T *volts, WARNING_T *warning, ERROR_T *error) {
	if (volts->value < CELL_WARN_VOLTAGE) {
		*warning = WARN_CELL_LOW_VOLTAGE;
	}

	if (volts->value < CELL_MIN_VOLTAGE) {
		error_set(ERROR_CELL_UNDER_VOLTAGE, &volts->error, HAL_GetTick());
	} else {
		error_unset(ERROR_CELL_UNDER_VOLTAGE, &volts->error);
	}

	if (volts->value > CELL_MAX_VOLTAGE) {
		error_set(ERROR_CELL_OVER_VOLTAGE, &volts->error, HAL_GetTick());
	} else {
		error_unset(ERROR_CELL_OVER_VOLTAGE, &volts->error);
	}

	*error = error_check_fatal(&volts->error, HAL_GetTick());
	ER_CHK(error);

End:;
}

/**
 * @brief		Checks that temperature is between its thresholds.
 *
 * @param		temp		The temperature
 * @param		error		The error return code
 */
void ltc6804_check_temperature(ER_UINT16_T *temp, ERROR_T *error) {
	if (temp->value >= CELL_MAX_TEMPERATURE) {
		error_set(ERROR_CELL_OVER_TEMPERATURE, &temp->error, HAL_GetTick());
	} else {
		error_unset(ERROR_CELL_OVER_TEMPERATURE, &temp->error);
	}

	*error = error_check_fatal(&temp->error, HAL_GetTick());
	ER_CHK(error);

End:;
}

/**
 * @brief		Wakes up all the devices connected to the isoSPI bus
 *
 * @param		hspi	The SPI configuration structure
 */
void _wakeup_idle(SPI_HandleTypeDef *hspi, bool apply_delay) {
	uint8_t data = 0xFF;
	ltc6804_enable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);

	HAL_SPI_Transmit(hspi, &data, 1, 1);

	ltc6804_disable_cs(hspi, CS_6820_GPIO_Port, CS_6820_Pin);
}

/**
 * @brief		This function is used to calculate the PEC value
 *
 * @param		len		Length of the data array
 * @param		data	Array of data
 */
uint16_t _pec15(uint8_t len, uint8_t data[]) {
	uint16_t remainder, address;
	remainder = 16;	 // PEC seed
	for (int i = 0; i < len; i++) {
		// calculate PEC table address
		address = ((remainder >> 7) ^ data[i]) & 0xff;
		remainder = (remainder << 8) ^ crcTable[address];
	}
	// The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
	return (remainder * 2);
}

/**
 * @brief	This function is used to convert the 2 byte raw data from the
 * 				LTC68xx to a 16 bit unsigned integer
 *
 * @param 	v_data	Raw data bytes
 *
 * @retval	Voltage [mV]
 */
uint16_t _convert_voltage(uint8_t v_data[]) { return v_data[0] + (v_data[1] << 8); }

/**
 * @brief		This function converts a voltage data from the zener sensor
 * 					to a temperature
 *
 * @param		volt	Voltage [mV]
 *
 * @retval	Temperature [C° * 100]
 */
uint16_t _convert_temp(uint16_t volt) {
	float voltf = volt * 0.0001;
	float temp;
	temp = -225.7 * voltf * voltf * voltf + 1310.6 * voltf * voltf - 2594.8 * voltf + 1767.8;
	return (uint16_t)(temp * 100);
}
