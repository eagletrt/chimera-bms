
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"

/* USER CODE BEGIN Includes */
//#include "stm32f4xx_hal_can.h"
#include "measurement.h"
#include "eeprom.h"
#include "can.h"
#include "ltc68xx.h"
#include "ltc1865.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


CAN_FilterConfTypeDef sFilter;
CanRxMsgTypeDef RxHeader;

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

///*----- i2c Write Function -----*/
//void i_write(unsigned position, unsigned int data){
//
//	int count=0;
//	if(data>255){
//		while(data>255){
//			data=data-255;
//			count++;
//		}
//	}
//
//	if(count>0){
//		//for writing to EEPROM
//		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
//		HAL_Delay(5);
//		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
//		HAL_Delay(5);
//	}else{
//		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
//		HAL_Delay(5);
//		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
//	    HAL_Delay(5);
//	}
//}
//
///*----- i2c Write Function -----*/
//unsigned int i_read(unsigned position){
//
//	int count=0;
//	unsigned int data=0;
//	//for reading to EEPROM
//	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
//	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
//	data=data+(count*255);
//	return data;
//
//}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */



uint16_t crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
                            0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                            0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                            0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                            0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                            0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                            0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                            0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                            0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                            0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                            0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                            0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                            0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                            0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                            0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                            0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                            0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                            0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                            0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                            0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                            0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                            0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                            0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                            };

/*!  Write the ltc6811 Sctrl register
This command will write the pwm registers of the ltc6811-1s
connected in a daisy chain stack. The pwm is written in descending
order so the last device's pwm is written first.
The function will calculate the needed PEC codes for the write data
and then transmit data to the ICs on a daisy chain.
Command Code:
-------------
|               |                           CMD[0]                              |                            CMD[1]                             |
|---------------|---------------------------------------------------------------|---------------------------------------------------------------|
|CMD[0:1]       |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|WRSCTRL:         |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   0   |   0   |
*/
//void ltc6811_wrsctrl(uint8_t nIC,           //!< number of ICs in the daisy chain
//                     uint8_t sctrl_reg,
//                     uint8_t sctrl[][6]
//                     ){ //0b00010100
//
//	uint8_t cmd[12];
//	uint16_t cmd_pec;
//	uint16_t cmd_pec1;
//	//uint8_t md_bits;
//
//	//md_bits = (MD & 0x02) >> 1;
//	//cmd[0] = md_bits + 0x02;
//	cmd[0]=0x00;
//	//md_bits = (MD & 0x01) << 7;
//	//cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
//	cmd[1]=0b00010100;
//
//	cmd_pec = pec15(2, cmd);
//	cmd[2] = (uint8_t)(cmd_pec >> 8);
//	cmd[3] = (uint8_t)(cmd_pec);
//	cmd[4]= 0b00010001;
//	cmd[5]= 0b00010001;
//	cmd[6]= 0b00010001;
//	cmd[7]= 0b10010001;
//	cmd[8]= 0b00010001;
//	cmd[9]= 0b00011001;
//	cmd_pec1 = pec15(2, cmd);
//	cmd[10] = (uint8_t)(cmd_pec1 >> 8);
//	cmd[11] = (uint8_t)(cmd_pec1);
//	wakeup_idle1();
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//	spi_write_array(12, cmd);
//	//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//}
//
///*!  Reads sctrl registers of a ltc6811 daisy chain
//uint8_t r_sctrl[][8] is a two dimensional array that the function stores the read pwm data. The sctrl data for each IC
//is stored in blocks of 8 bytes with the pwm data of the lowest IC on the stack in the first 8 bytes
//block of the array, the second IC in the second 8 byte etc. Below is an table illustrating the array organization:
//@return int8_t, PEC Status.
//0: Data read back has matching PEC
//-1: Data read back has incorrect PEC
//Command Code:
//-------------
//|CMD[0:1]   |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
//|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
//|rdsctrl:   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   1   |   0   |
//*/
//uint8_t ltc6811_rdsctrl(uint8_t nIC, 			//!< number of ICs in the daisy chain
//                        uint8_t sctrl_reg,
//                        uint8_t r_sctrl[][8]    //!< a two dimensional array that the function stores the read pwm data
//			                      ){
//
//	uint8_t cmd[12];
//	uint8_t cmd_pec;
//	uint16_t cmd_pec1;
//	uint8_t* d;
//	//uint8_t md_bits;
//
//	for(int i=0;i<nIC;i++){
//	  //md_bits = (MD & 0x02) >> 1;
//	  //cmd[0] = md_bits + 0x02;
//	  cmd[0]=0x00;
//	  //md_bits = (MD & 0x01) << 7;
//	  //cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
//	  cmd[1]=0b00010110;
//
//	  cmd_pec = pec15(2, cmd);
//      cmd[2] = (uint8_t)(cmd_pec >> 8);
//	  cmd[3] = (uint8_t)(cmd_pec);
//
//	  cmd[4]= 0b00010001;
//	  cmd[5]= 0b00010001;
//      cmd[6]= 0b00010001;
//	  cmd[7]= 0b10010001;
//	  cmd[8]= 0b00010001;
//	  cmd[9]= 0b00011001;
//      cmd_pec1 = pec15(2, cmd);
//
//	  cmd[10] = (uint8_t)(cmd_pec1 >> 8);
//	  cmd[11] = (uint8_t)(cmd_pec1);
//
//	  uint8_t* r;
//
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//      spi_write_read(cmd,12,r,12);      //  DA CONTROLLARE CON PCB
//	  d[i]=r;
//	  //HAL_SPI_Transmit(&hspi1, cmd, 4,10);
//	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
//	}
//}
//
///*!  Start Sctrl data communication
//This command will start the sctrl pulse communication over the spins
//*/
//void LTC6811_stsctrl(){
//
//}



  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* SETTING CAN */
  sFilter.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilter.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilter.FilterIdLow = 0;
  sFilter.FilterIdHigh = 0;
  sFilter.FilterMaskIdHigh = 0;
  sFilter.FilterMaskIdLow = 0;
  sFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  sFilter.BankNumber = 0;
  sFilter.FilterScale = CAN_FILTERSCALE_16BIT;
  sFilter.FilterActivation = ENABLE;
  HAL_CAN_ConfigFilter(&hcan, &sFilter);

  HAL_CAN_Init(&hcan);

  int TOT_IC=12; // number of daisy chain
  int CELL_CH=9;

  uint8_t NUM_RX_BYT = 8;
  uint8_t BYT_IN_REG = 6;
  uint8_t CELL_IN_REG = 3;
  uint8_t NUM_CV_REG = 3;
  uint8_t *cell_data;
  uint16_t cell_codes[TOT_IC][CELL_CH];
  uint16_t cell_codes_temp[TOT_IC][CELL_CH];
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t pec_error=0;
  int counterCicle = 0;
  float *av_temp1 = 0;
  float *av_temp2 = 0;
  float *av_temp3 = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */



  	int id = 0;

 	 // GPIO Logic
 	 //Precharge off
 	// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
 	 GPIO_PinState ShutDown_Status;
 	 ShutDown_Status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
 	 if(ShutDown_Status == GPIO_PIN_SET){
 		 uint8_t *totalPack;
 		 uint8_t *tractiveVoltage;
 		 while(totalPack != tractiveVoltage){
 			 menu_1_read_single_ended(hspi1, tractiveVoltage, totalPack);
 		 }
 		 // Monitorare Total voltage e tractive system voltage
 		 // Quando sono uguali ---> precharge on
 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
 		 HAL_Delay(1);
 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
 	 }

 	 if(ShutDown_Status == GPIO_PIN_RESET){
 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
 		 //CAN SEND TS OFF
 	 }

 	 //QUANDO ARRIVA IL MESSAGGIO CAN TS ON ----> ATTIVA IL TRACTIVE SYSTEM

 	 //QUANDO ARRIVA IL MESSAGGIO CAN TS OFF ----> DISATTIVA IL TRACTIVE SYSTEM

  	 /* ----- Voltages ------*/
  	 ltc6804_adcv(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
  	 //ltc6804_pollAdc(hspi1);
  	 HAL_Delay(10);
	 uint8_t data_counter = 0;
	 for(uint8_t current_ic = 0 ; current_ic < TOT_IC; current_ic++){

			 ltc6804_rdcv_reg(current_ic, TOT_IC, cell_data, hspi1);
	 	 	 uint16_t *voltages;
	 	 	 array_voltages(voltages, cell_data);
	 	 	 for(int i = 0; i < 9; i++){
	 	 		 cell_codes[current_ic][i] = voltages[i];
	 	  	 }
	 }
	 uint16_t *max_vol;
	 uint16_t *min_vol;
	 float *average_vol;
	 max_min_voltages(cell_codes, max_vol, min_vol, average_vol);
	 //Can Messages

	 /* ----- Temperatures -----*/
	 uint16_t *temp = 0;
	 //odd temp
	 ltc6804_address_temp_odd(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	 ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	 HAL_Delay(10);
	 for(uint8_t current_ic = 0 ; current_ic < TOT_IC; current_ic++){
		 	 ltc6804_rdcv_reg(current_ic, TOT_IC, cell_data, hspi1);
		 	 array_temp_odd(temp, cell_data);

		 	 cell_codes_temp[current_ic][0] = temp[0];
		 	 cell_codes_temp[current_ic][2] = temp[2];
		 	 cell_codes_temp[current_ic][4] = temp[4];
		 	 cell_codes_temp[current_ic][6] = temp[6];
		 	 cell_codes_temp[current_ic][8] = temp[8];

	 }
	 //ltc6804_rdcv_temp(...);
	 //convert_temp();

	 //even temp
	 ltc6804_address_temp_even(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	 ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	 HAL_Delay(10);
	 for(uint8_t current_ic = 0 ; current_ic < TOT_IC; current_ic++){
	 		 ltc6804_rdcv_reg(current_ic, TOT_IC, cell_data, hspi1);
	 		 array_temp_even(temp, cell_data);

	 		 cell_codes_temp[current_ic][1] = temp[1];
	 		 cell_codes_temp[current_ic][3] = temp[3];
	 		 cell_codes_temp[current_ic][5] = temp[5];
	 		 cell_codes_temp[current_ic][7] = temp[7];

	 }
	 // Controllo Temperatura massima
	 uint16_t *max_temp = 0;
	 if(counterCicle % 3 == 0){
		 max_ave_temp(cell_codes_temp, max_temp, av_temp1);
	 }
	 if(counterCicle % 3 == 1){
	 	 max_ave_temp(cell_codes_temp, max_temp, av_temp2);
	 }
	 if(counterCicle % 3 == 2){
	 	 max_ave_temp(cell_codes_temp, max_temp, av_temp3);
	 }
	 float average = 0;
	 if(counterCicle > 2){

	 average = (*av_temp1 + *av_temp2 + *av_temp3)/3;



	 if(average < 60 && *max_vol*0.0001f < 4.20 && *min_vol*0.0001f > 2.80){
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	 }
	 }
	 float **converted_temp;
	 for(int i = 0; i < 12; i++){
		 for(int j = 0; j < 9; j++){
			 converted_temp[i][j] = convert_temp(cell_codes_temp[i][j]);
		 }
	 }

	 //CAN MESSAGES
	 uint16_t tot_vol = total_pack_voltage(cell_codes);

	 CAN_Send(id,(uint8_t)max_vol, 8, hcan);
	 CAN_Send(id,(uint8_t)min_vol, 8, hcan);
	 CAN_Send(id, tot_vol, 8, hcan);
	 CAN_Send(id, (tot_vol << 8), 8, hcan);
	 CAN_Send(id,*max_temp, 8, hcan);
	 CAN_Send(id,*max_temp << 8, 8, hcan);
	 CAN_Send(id,(uint8_t)average, 8, hcan);
	 for(int i = 0; i < TOT_IC; i++){
		 for(int j = 0; j < 9; j++){
			 CAN_Send(id, (uint8_t)cell_codes[i][j], 8, hcan);
			 CAN_Send(id, (uint8_t)cell_codes[i][j] << 8, 8, hcan);
			 CAN_Send(id, (uint8_t)cell_codes_temp[i][j], 8, hcan);
			 CAN_Send(id, (uint8_t)cell_codes_temp[i][j] << 8, 8, hcan);
		 }
	 }


	 counterCicle = counterCicle + 1;
 }

  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_MultiModeTypeDef multimode;
  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the ADC multi-mode 
    */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* CAN init function */
static void MX_CAN_Init(void)
{

  hcan.Instance = CAN;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SJW = CAN_SJW_1TQ;
  hcan.Init.BS1 = CAN_BS1_6TQ;
  hcan.Init.BS2 = CAN_BS2_1TQ;
  hcan.Init.TTCM = DISABLE;
  hcan.Init.ABOM = DISABLE;
  hcan.Init.AWUM = DISABLE;
  hcan.Init.NART = DISABLE;
  hcan.Init.RFLM = DISABLE;
  hcan.Init.TXFP = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PrechargeEnded_Pin|TS_ON_Pin|BMS_Fault_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_ADC_PackV_Pin|CS_6820_Pin|CS_SDCard_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EEPromWc_GPIO_Port, EEPromWc_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PrechargeEnded_Pin CS_ADC_PackV_Pin CS_6820_Pin CS_SDCard_Pin 
                           TS_ON_Pin BMS_Fault_Pin */
  GPIO_InitStruct.Pin = PrechargeEnded_Pin|CS_ADC_PackV_Pin|CS_6820_Pin|CS_SDCard_Pin 
                          |TS_ON_Pin|BMS_Fault_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SDCard_Det_Pin */
  GPIO_InitStruct.Pin = SDCard_Det_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SDCard_Det_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ShutDownSt_Pin */
  GPIO_InitStruct.Pin = ShutDownSt_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ShutDownSt_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EEPromWc_Pin */
  GPIO_InitStruct.Pin = EEPromWc_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EEPromWc_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//
//
//int CAN_Receive(uint8_t *DataRx, int size){
//	=======NON VA
//
//	if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_FIFO0) != 0){
//		HAL_CAN_GetRxMessage(&hcan, CAN_FIFO0, &RxHeader, DataRx);
//	}
//
//	int id = RxHeader.StdId;
//
//	return id;
//}
//*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
