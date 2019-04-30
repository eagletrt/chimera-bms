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
uint16_t pec15(uint8_t len, uint8_t data[]) {

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




/*void ltc6804_rdcv_temp(Cell cells[],
					   SPI_HandleTypeDef *hspi){

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];
	cmd[0] = (uint8_t)0x80 + 8*IC_COUNT;

	wakeup_idle(hspi);

	for(uint8_t parity=0;parity<1;parity++){

	// ---- Celle 1, 2, 3
	cmd[1] = (uint8_t)0x04;
	cmd_pec = pec15(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 100);
	HAL_SPI_Receive(hspi, data, 8, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	if(pec15(6, data) == (uint16_t) (data[6]*256 + data[7])){


			if(parity==0){

				cells[IC_COUNT*9+1].temperature = convert_temp(convert_voltage(&data[2]));
				cells[IC_COUNT*9+1].temperature_faults = 0;

			}
			else{

				cells[IC_COUNT*9].temperature = convert_temp(convert_voltage(&data[0]));
				cells[IC_COUNT*9+2].temperature = convert_temp(convert_voltage(&data[4]));
				cells[IC_COUNT*9].temperature_faults = 0;
				cells[IC_COUNT*9+2].temperature_faults = 0;


		}

	}
	else{

		if(parity==0)
			cells[IC_COUNT*9+1].temperature_faults++;
		else{

			cells[IC_COUNT*9].temperature_faults++;
			cells[IC_COUNT*9+2].temperature_faults++;

		}

	}

	// ---- Celle 4, 5, /
	cmd[1] = 0x06;
	cmd_pec = pec15(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 100);
	HAL_SPI_Receive(hspi, data, 8, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	if(pec15(6, data) == (uint16_t) (data[6]*256 + data[7])){

		if(parity==0){

			cells[IC_COUNT*9+3].temperature = convert_temp(convert_voltage(&data[0]));
			cells[IC_COUNT*9+3].temperature_faults = 0;

		}
		else{

			cells[IC_COUNT*9+4].temperature = convert_temp(convert_voltage(&data[2]));
			cells[IC_COUNT*9+4].temperature_faults = 0;

		}

	}
	else{

		if(parity==0)
			cells[IC_COUNT*9+3].temperature_faults++;
		else
			cells[IC_COUNT*9+4].temperature_faults++;

	}

	// ---- Celle 6, 7, 8
	cmd[1] = 0x08;
	cmd_pec = pec15(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	// CS LOW
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, cmd, 4, 100);
	HAL_SPI_Receive(hspi, data, 8, 100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	if(pec15(6, data) == (uint16_t) (data[6]*256 + data[7])){

		if(parity==0){

			cells[IC_COUNT*9+5].temperature = convert_temp(convert_voltage(&data[0]));
			cells[IC_COUNT*9+7].temperature = convert_temp(convert_voltage(&data[4]));
			cells[IC_COUNT*9+5].temperature_faults = 0;
			cells[IC_COUNT*9+7].temperature_faults= 0;

		}
		else{

			cells[IC_COUNT*9+6].temperature = convert_temp(convert_voltage(&data[2]));
			cells[IC_COUNT*9+6].temperature_faults = 0;

		}

	}
	else{

		if(parity==0){

			cells[IC_COUNT*9+5].temperature_faults++;
			cells[IC_COUNT*9+7].temperature_faults++;

		}
		else
			cells[IC_COUNT*9+6].temperature_faults++;

	}

	// ---- Celle 9, /, /
	if(parity==1){

		cmd[1] = 0x0A;
		cmd_pec = pec15(2, cmd);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		// CS LOW
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_SPI_Transmit(hspi, cmd, 4, 100);
		HAL_SPI_Receive(hspi, data, 8, 100);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		if(pec15(6, data) == (uint16_t) (data[6]*256 + data[7])){

			cells[IC_COUNT*9+8].temperature = convert_temp(convert_voltage(&data[0]));
			cells[IC_COUNT*9+8].temperature_faults = 0;

		}
		else
			cells[IC_COUNT*9+8].temperature_faults++;

	}
	}
	return;

}*/


/**
 * @brief		Reads the data form the LTC68xx and updates the cell temperatures
 * @param		array of cells
 * @param		hspi pointer to a SPI_HandleTypeDef structure that contains
 * 				the configuration information for SPI module.
 */
void ltc6804_rdcv_temp(CELL_T cells[], SPI_HandleTypeDef *hspi) {
	wakeup_idle(hspi);
	HAL_Delay(10);

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	uint8_t ic;
	uint8_t reg;
	uint8_t parity;

	for (parity = 0; parity <= 1; parity++) {
		ltc6804_command_temperatures(1, parity, hspi);	// switch between even and odd
		HAL_Delay(100);

		for (ic = 0; ic < LTC6804_COUNT; ic++) {
			uint8_t count = 0;
			uint8_t ic_count = ic * LTC6804_CELL_COUNT;

			cmd[0] = (uint8_t) 0x80U + 8U * ic;

			for (reg = 0; reg < LTC6804_REG_COUNT; reg++) {
				cmd[1] = (uint8_t) rdcv_cmd[reg];
				cmd_pec = pec15(2, cmd);
				cmd[2] = (uint8_t) (cmd_pec >> 8);
				cmd[3] = (uint8_t) (cmd_pec);

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
				HAL_SPI_Transmit(hspi, cmd, 4, 100);
				HAL_SPI_Receive(hspi, data, 8, 100);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

				uint8_t pec = pec15(6, data) == (uint16_t) (data[6] * 256 + data[7]);


				switch(reg){
				case 0:
				case 2:
					if (parity) {

						if (pec) {
							cells[ic_count + reg*LTC6804_REG_CELL_COUNT+1].temperature = convert_temp(convert_voltage(&data[2]));
							cells[ic_count + reg*LTC6804_REG_CELL_COUNT+1].temperature_faults = 0;
						} else {
							cells[ic_count + reg*LTC6804_REG_CELL_COUNT+1].temperature_faults++;
						}

					} else {
						if (pec) {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature =	convert_temp(convert_voltage(&data[0]));
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults = 0;

							cells[ic_count + reg * LTC6804_REG_CELL_COUNT+2].temperature =	convert_temp(convert_voltage(&data[4]));
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT+2].temperature_faults = 0;
						} else {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults++;
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT+2].temperature_faults++;
						}

					}
					break;
				case 1:
					if (parity) {

						if (pec) {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT + 1].temperature =	convert_temp(convert_voltage(&data[2]));
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT + 1].temperature_faults = 0;
						} else {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT + 1].temperature_faults++;
						}

					} else {
						if (pec) {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature =	convert_temp(convert_voltage(&data[0]));
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults =0;
						} else {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults++;
						}

					}
					break;
				case 3:
					if(!parity){
						if (pec) {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature = convert_temp(convert_voltage(&data[0]));
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults = 0;
						} else {
							cells[ic_count + reg * LTC6804_REG_CELL_COUNT].temperature_faults++;
						}
					}
				}

//				if (parity) {
//
//					count++; // Skipping the first cell
//
//					if (cell_distribution[reg * IC_REG_CELL_COUNT + 1]) {
//						if (pec) {
//							cells[ic_count + count].temperature = ic_count+count;//convert_temp(convert_voltage(&data[2]));
//							cells[ic_count + count].temperature_faults = 0;
//						} else {
//							cells[ic_count + count].temperature_faults++;
//						}
//
//						if(cell_distribution[reg * IC_REG_CELL_COUNT + 2]){
//							count++;
//						}
//					}
//
//					count++; // Skipping the last cell
//				} else {
//
//					if (cell_distribution[reg * IC_REG_CELL_COUNT]) {
//						if (pec) {
//							cells[ic_count + count].temperature = convert_temp(convert_voltage(&data[0]));
//							cells[ic_count + count].temperature_faults = 0;
//						} else {
//							cells[ic_count + count].temperature_faults++;
//						}
//
//						count++;
//					}
//
//						count++; // Skipping the middle cell
//
//					if (cell_distribution[reg * IC_REG_CELL_COUNT + 2]) {
//						if (pec) {
//							cells[ic_count + count].temperature = convert_temp(convert_voltage(&data[4]));
//							cells[ic_count + count].temperature_faults = 0;
//						} else {
//							cells[ic_count + count].temperature_faults++;
//						}
//
//						count++;
//					}
//					else{
//
//					}
//				}
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
//void ltc6804_rdcv_voltages(CELL_T cells[], SPI_HandleTypeDef *hspi) {
//	wakeup_idle(hspi);
//	HAL_Delay(100);
//	ltc6804_adcv(0, hspi);
//	HAL_Delay(100);
//
//	uint8_t cmd[4];
//	uint16_t cmd_pec;
//	uint8_t data[8];
//
//	uint8_t ic;
//	uint8_t reg;
//
//	for (ic = 0; ic < LTC6804_COUNT; ic++) {
//		// if cell_distribution is 0 the count is not increased
//		uint8_t count = 0;
//		uint8_t ic_count = ic * LTC6804_CELL_COUNT;
//
//		cmd[0] = (uint8_t) 0x80 + 8 * ic;
//
//		for (reg = 0; reg < LTC6804_REG_COUNT; reg++) {
//
//			cmd[1] = (uint8_t) rdcv_cmd[reg];
//			cmd_pec = pec15(2, cmd);
//			cmd[2] = (uint8_t) (cmd_pec >> 8);
//			cmd[3] = (uint8_t) (cmd_pec);
//
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
//			HAL_SPI_Transmit(hspi, cmd, 4, 100);
//			HAL_SPI_Receive(hspi, data, 8, 100);
//			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
//
//			uint8_t pec = pec15(6, data) == (uint16_t) (data[6] * 256 + data[7]);
//			uint8_t cell;
//			for (cell = 0; cell < LTC6804_REG_CELL_COUNT; cell++) {
//
//				if (cell_distribution[reg * LTC6804_REG_CELL_COUNT + cell]) {
//
//					if (pec) {
//						cells[ic_count + count].voltage = convert_voltage(&data[2 * cell]);
//						cells[ic_count + count].voltage_faults = 0;
//
//					} else {
//						cells[ic_count + count].voltage_faults++;
//					}
//					count++;
//				}
//			}
//		}
//	}
//}




