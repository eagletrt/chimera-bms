/*
 * ltc68xx.c

 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */

#include "ltc68xx.h"

/*----- SPI Read Function -----*/
uint8_t spi_read_byte(uint8_t tx_dat,SPI_HandleTypeDef hspi1)
	{
	    uint8_t data;

	    if ( HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&data, 1, HAL_MAX_DELAY) == HAL_OK )
	    {
	        return(data);
	    }
	    return(-1);
}

/*----- Data Lenght Counter -----*/
int osal_DataLength( char *pString ){
	  return (int)( strlen( pString ) );
}

/*----- SPI Write Function (?) -----*/
void spi_write_read(uint8_t tx_Data[],	// Array of data to be written on SPI port
			        uint8_t tx_len, 	// Length of the tx data arry
			        uint8_t *rx_data,	// Input: array that will store the data read by the SPI port
			        uint8_t rx_len,	// Option: number of bytes to be read from the SPI port
					SPI_HandleTypeDef hspi1
			        ){
	for (uint8_t i = 0; i < tx_len; i++)
		  {
				// spi_write(tx_Data[i]);
				HAL_SPI_Transmit(&hspi1, (uint8_t*)&tx_Data[i], 1, 100);
		  }

	for (uint8_t i = 0; i < rx_len; i++)
		  {
	            HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&rx_data[i], 1, 100);
		  }
}

uint16_t pec15(uint8_t len,uint8_t* data,uint16_t crcTable[] ){

    uint16_t remainder,address;
	remainder = 16;					// PEC seed
	for (int i = 0; i < len; i++)
		{
			address = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
			remainder = (remainder << 8 ) ^ crcTable[address];
		}
    return (remainder*2);//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}
void wakeup_idle1(SPI_HandleTypeDef hspi1){

    // output_low(LTC6811_CS);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_read_byte(0xff,hspi1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

	//HAL_Delay(10);
}

void spi_write_array(uint8_t len, 		// Option: Number of bytes to be written on the SPI port
                     uint8_t data[] ,	// Array of bytes to be written on the SPI port
					 SPI_HandleTypeDef hspi1
			         )
	{
		uint8_t ret_val;
	    for ( int i = 0; i < len; i++ )
		    {
		        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&data[i], &ret_val, 1, 100);
		    }
}

void wakeup_sleep(SPI_HandleTypeDef hspi1){

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    // cs_low(GPIO_PIN_9);
    HAL_SPI_Transmit(&hspi1, 0xFF,1, 0xFF);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(1);
}
void max_min_voltages(uint16_t cell_codes[12][9], uint16_t* max_vol, uint16_t *min_vol, float *average_vol){

	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 9; j++){
			if(i==0 && j==0){
				*max_vol = cell_codes[i][j];
				if(*max_vol*0.0001f > 4.20){
					//SHUT DOWN
				}
				*min_vol = cell_codes[i][j];
				if(*min_vol*0.0001f < 2.80){
					//SHUT DOWN
				}
			}else{
				if(cell_codes[i][j] < *min_vol){
					*min_vol = cell_codes[i][j];
					if(*min_vol*0.0001f < 2.80){
						//SHUT DOWN
					}
				}
				if(cell_codes[i][j] > *max_vol){
					*max_vol = cell_codes[i][j];
					if(*max_vol*0.0001f > 4.20){
						//SHUT DOWN
					}
				}
			}

		}
	}
	// Average
	uint16_t sum = 0;
	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 9; j++){
			sum = sum + cell_codes[i][j];
		}
	}
	*average_vol = sum*0.0001f / 108;
}

void array_voltages(uint16_t *voltages, uint8_t *cell_data){
	voltages[0] = cell_data[0] + (cell_data[1] << 8);
	voltages[1] = cell_data[2] + (cell_data[3] << 8);
	voltages[2] = cell_data[4] + (cell_data[5] << 8);
	voltages[3] = cell_data[8] + (cell_data[9] << 8);
	voltages[4] = cell_data[10] + (cell_data[11] << 8);
	voltages[5] = cell_data[16] + (cell_data[17] << 8);
	voltages[6] = cell_data[18] + (cell_data[19] << 8);
	voltages[7] = cell_data[20] + (cell_data[21] << 8);
	voltages[8] = cell_data[24] + (cell_data[25] << 8);
}
void array_temp_odd(uint16_t *temp, uint8_t *cell_data){
	temp[0] = cell_data[0] + (cell_data[1] << 8);
	//temp[1] = cell_data[2] + (cell_data[3] << 8);
	temp[2] = cell_data[4] + (cell_data[5] << 8);
	//temp[3] = cell_data[8] + (cell_data[9] << 8);
	temp[4] = cell_data[10] + (cell_data[11] << 8);
	//temp[5] = cell_data[16] + (cell_data[17] << 8);
	temp[6] = cell_data[18] + (cell_data[19] << 8);
	//temp[7] = cell_data[20] + (cell_data[21] << 8);
	temp[8] = cell_data[24] + (cell_data[25] << 8);
}
void array_temp_even(uint16_t *temp, uint8_t *cell_data){
	//temp[0] = cell_data[0] + (cell_data[1] << 8);
	temp[1] = cell_data[2] + (cell_data[3] << 8);
	//temp[2] = cell_data[4] + (cell_data[5] << 8);
	temp[3] = cell_data[8] + (cell_data[9] << 8);
	//temp[4] = cell_data[10] + (cell_data[11] << 8);
	temp[5] = cell_data[16] + (cell_data[17] << 8);
	//temp[6] = cell_data[18] + (cell_data[19] << 8);
	temp[7] = cell_data[20] + (cell_data[21] << 8);
	//temp[8] = cell_data[24] + (cell_data[25] << 8);
}
void max_ave_temp(uint16_t cell_codes[12][9], uint16_t* max_temp, float *average_temp){
	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 9; j++){
			if(i==0 && j==0){
				*max_temp = cell_codes[i][j];
				if(*max_temp*0.0001f > 65.00){
					//SHUT DOWN
				}
			}else{
			if(cell_codes[i][j] > *max_temp){
					*max_temp = cell_codes[i][j];
					if(*max_temp*0.0001f > 65.00){
						//SHUT DOWN
					}
				}
			}
		}

	}

	// Average
	uint16_t sum = 0;
	for(int i = 0; i < 12; i++){
		for(int j = 0; j < 9; j++){
			sum = sum + cell_codes[i][j];
		}
	}
	*average_temp = sum*0.0001f / 108;
}
uint16_t total_pack_voltage(uint16_t cell_codes[12][9]){
	uint16_t sum = 0;
		for(int i = 0; i < 12; i++){
			for(int j = 0; j < 9; j++){
				sum = sum + cell_codes[i][j];
			}
		}
	return sum;
}
/*----- Read the raw data from the ltc6804 cell temperature register normal-----*/
void ltc6804_rdcv_temp(uint8_t ic_n,				// Number of the current ic
				  uint8_t total_ic, 		// The number of ICs in the
			      uint8_t *rx_data,			// An array of the unparsed cell codes
				  SPI_HandleTypeDef hspi1
			          ){

	const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t *data = 0;
	ic_n = (uint8_t)0x80 + (2^3)*ic_n;

	// ---- Celle 1, 2, 3
		cmd[0] = ic_n;
		cmd[1] = 0x04;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
			rx_data[i] = data[i];
		}

	// ---- Celle 4, 5, /
		cmd[0] = ic_n;
		cmd[1] = 0x06;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 8] = data[i];
		}

	// ---- Celle 6, 7, 8
		cmd[0] = ic_n;
	    cmd[1] = 0x08;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
	    //write_read del gruppo
	    wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
	    // Output_low
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	    spi_write_read(cmd,4,data,8,hspi1);
	    // Output_high
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 16] = data[i];
		}

	// ---- Celle 9, /, /
		cmd[0] = ic_n;
		cmd[1] = 0x0A;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 24] = data[i];
		}

}
/*----- Read the raw data from the ltc6804 cell voltage register normal-----*/
void ltc6804_rdcv_reg(uint8_t ic_n,				// Number of the current ic
					  uint8_t total_ic, 		// The number of ICs in the
			          uint8_t *rx_data,			// An array of the unparsed cell codes
					  SPI_HandleTypeDef hspi1
			          ){

	const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t *data;
	ic_n = (uint8_t)0x80 + (2^3)*ic_n;

	// ---- Celle 1, 2, 3
		cmd[0] = ic_n;
		cmd[1] = 0x04;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);

		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
			rx_data[i] = data[i];
		}

	// ---- Celle 4, 5, /
		cmd[0] = ic_n;
		cmd[1] = 0x06;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 8] = data[i];
		}

	// ---- Celle 6, 7, 8
		cmd[0] = ic_n;
	    cmd[1] = 0x08;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
	    //write_read del gruppo
	    wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
	    // Output_low
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	    spi_write_read(cmd,4,data,8,hspi1);
	    // Output_high
	    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 16] = data[i];
		}

	// ---- Celle 9, /, /
		cmd[0] = ic_n;
		cmd[1] = 0x0A;
		cmd_pec = pec15(2, cmd,crcTable);
		cmd[2] = (uint8_t)(cmd_pec >> 8);
		cmd[3] = (uint8_t)(cmd_pec);
		//write_read del gruppo
		wakeup_idle1(hspi1); 			//This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
		// Output_low
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_write_read(cmd,4,data,8,hspi1);
		// Output_high
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		for(int i = 0; i < 8; i++){
				rx_data[i + 24] = data[i];
		}

}
//
///*----- Read the raw data from the ltc6811 cell voltage register deasy-----*/
//void ltc6804_rdcv_reg(uint8_t reg, 			// Determines which cell voltage register is read back
//			          uint8_t total_ic, 	// The number of ICs in the
//			          uint8_t data[] ,		// An array of the unparsed cell codes
//					  SPI_HandleTypeDef hspi1
//			          ){
//
//	const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
//	uint8_t cmd[4];
//	uint16_t cmd_pec;
//
//	if (reg == 1)     //1: RDCVA
//	{
//		cmd[0] = reg;
//		cmd[1] = 0x04;
//	}else if (reg == 2) //2: RDCVB
//	{
//		cmd[0] = reg;
//		cmd[1] = 0x06;
//	}else if (reg == 3) //3: RDCVC
//	{
//		cmd[0] = reg;
//	    cmd[1] = 0x08;
//	}else if (reg == 4) //4: RDCVD
//	{
//		cmd[0] = reg;
//		cmd[1] = 0x0A;
//	}else if (reg == 5) //4: RDCVE
//	{
//		cmd[0] = reg;
//		cmd[1] = 0x09;
//	}else if (reg == 6) //4: RDCVF
//	{
//		cmd[0] = reg;
//		cmd[1] = 0x0B;
//
//	}
//
//
//	cmd_pec = pec15(2, cmd,crcTable);
//	cmd[2] = (uint8_t)(cmd_pec >> 8);
//	cmd[3] = (uint8_t)(cmd_pec);
//
//	wakeup_idle1(hspi1); //This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.
//    // output_low(LTC6811_CS);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//	spi_write_read(cmd,4,data,8,hspi1);
//	//HAL_SPI_TransmitReceive(&hspi1,cmd,data,8,100);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//	// output_high(LTC6811_CS);
//}
void ltc6804_address_temp_odd(uint8_t MD, 		//!< ADC Conversion Mode
                  	  	  	  uint8_t DCP, 		//!< Controls if Discharge is permitted during conversion
							  uint8_t CH ,		//!< Sets which Cell channels are converted
							  SPI_HandleTypeDef hspi1
			      	  	  	  ){

	uint8_t cmd[4];
	uint16_t cmd_pec;

	//md_bits = (MD & 0x02) >> 1;
	//cmd[0] = md_bits + 0x02;
	cmd[0] = 0x00;
	//md_bits = (MD & 0x01) << 7;
	//cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	cmd[1] = 0x01;

	cmd_pec = pec15(2, cmd,crcTable);

	cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    wakeup_idle1(hspi1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_write_array(4, cmd, hspi1);
	//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

	uint8_t cfng[8];
	cfng[0] = 0x00;
	cfng[1] = 0x00;
	cfng[2] = 0x00;
	cfng[3] = 0x00;
	cfng[4] = 0x95;
	cfng[5] = 0x02;
	cmd_pec = pec15(6, cfng, crcTable);
	cfng[6] = (uint8_t)(cmd_pec >> 8);
	cfng[7] = (uint8_t)(cmd_pec);
}

void ltc6804_address_temp_even(uint8_t MD, 		//!< ADC Conversion Mode
                  	  	  	  uint8_t DCP, 		//!< Controls if Discharge is permitted during conversion
							  uint8_t CH ,		//!< Sets which Cell channels are converted
							  SPI_HandleTypeDef hspi1
			      	  	  	  ){

	uint8_t cmd[4];
	uint16_t cmd_pec;

	//md_bits = (MD & 0x02) >> 1;
	//cmd[0] = md_bits + 0x02;
	cmd[0] = 0x00;
	//md_bits = (MD & 0x01) << 7;
	//cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	cmd[1] = 0x01;

	cmd_pec = pec15(2, cmd,crcTable);

	cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    wakeup_idle1(hspi1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_write_array(4, cmd, hspi1);
	//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

	uint8_t cfng[8];
	cfng[0] = 0x00;
	cfng[1] = 0x00;
	cfng[2] = 0x00;
	cfng[3] = 0x00;
	cfng[4] = 0x4A;
	cfng[5] = 0x01;
	cmd_pec = pec15(6, cfng, crcTable);
	cfng[6] = (uint8_t)(cmd_pec >> 8);
	cfng[7] = (uint8_t)(cmd_pec);
}
float convert_temp(uint16_t volt){
	float temp;
	temp = 9*(10^(-7))*((volt^3)*0.0001f) - 6*(10^(-5))*((volt^2)*0.0001f) - 0.0108*volt*0.0001f + 2,1624;
	return temp;
}
void ltc6804_adcv_temp(uint8_t MD, 		//!< ADC Conversion Mode
                  uint8_t DCP, 		//!< Controls if Discharge is permitted during conversion
                  uint8_t CH ,		//!< Sets which Cell channels are converted
				  SPI_HandleTypeDef hspi1
			      ){

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t md_bits;

	//md_bits = (MD & 0x02) >> 1;
	//cmd[0] = md_bits + 0x02;
	cmd[0] = 0x03;
	//md_bits = (MD & 0x01) << 7;
	//cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	cmd[1] = 0x70;

	cmd_pec = pec15(2, cmd,crcTable);

	cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    wakeup_idle1(hspi1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_write_array(4, cmd, hspi1);
	//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}
void ltc6804_adcv(uint8_t MD, 		//!< ADC Conversion Mode
                  uint8_t DCP, 		//!< Controls if Discharge is permitted during conversion
                  uint8_t CH ,		//!< Sets which Cell channels are converted
				  SPI_HandleTypeDef hspi1
			      ){

	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t md_bits;

	//md_bits = (MD & 0x02) >> 1;
	//cmd[0] = md_bits + 0x02;
	cmd[0] = 0x03;
	//md_bits = (MD & 0x01) << 7;
	//cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	cmd[1] = 0x60;

	cmd_pec = pec15(2, cmd,crcTable);

	cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);

    wakeup_idle1(hspi1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_write_array(4, cmd, hspi1);
	//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}



uint32_t ltc6804_pollAdc(SPI_HandleTypeDef hspi1){

	uint32_t counter = 0;
	uint8_t finished = 0;
	uint8_t current_time = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x14;
	cmd_pec = pec15(2, cmd, crcTable);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	wakeup_idle1(hspi1); //This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.

	// Output_low(LTC6811_CS);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	spi_write_array(4,cmd,hspi1);

	while ((counter<200000)&&(finished == 0)){
	  uint8_t rx;
      // Current_time= HAL_SPI_Receive(&hspi1,rx,0xFF,100);
	  current_time= spi_read_byte(0xFF,hspi1);
	  if (current_time>0){
		finished = 1;
	  }else{
		counter = counter + 10;
	  }
    }

	// Output_high(LTC6811_CS);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

    return(counter);
}
