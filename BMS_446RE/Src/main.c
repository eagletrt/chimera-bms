
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
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

#define MD_422HZ_1KHZ 0
#define MD_27KHZ_14KHZ 1
#define MD_7KHZ_3KHZ 2
#define MD_26HZ_2KHZ 3

#define ADC_OPT_ENABLED 1
#define ADC_OPT_DISABLED 0

#define CELL_CH_ALL 0
#define CELL_CH_1and7 1
#define CELL_CH_2and8 2
#define CELL_CH_3and9 3
#define CELL_CH_4and10 4
#define CELL_CH_5and11 5
#define CELL_CH_6and12 6

#define SELFTEST_1 1
#define SELFTEST_2 2

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6

#define STAT_CH_ALL 0
#define STAT_CH_SOC 1
#define STAT_CH_ITEMP 2
#define STAT_CH_VREGA 3
#define STAT_CH_VREGD 4

#define DCP_DISABLED 0
#define DCP_ENABLED 1

#define PULL_UP_CURRENT 1
#define PULL_DOWN_CURRENT 0

#define CELL_CHANNELS 12
#define AUX_CHANNELS 6
#define STAT_CHANNELS 4
#define CELL 1
#define AUX 2
#define STAT 3


#define M24M02DRC_1_DATA_ADDRESS 0x50 // Address of the first 1024 page M24M02DRC EEPROM data buffer, 2048 bits (256 8-bit bytes) per page
#define M24M02DRC_1_IDPAGE_ADDRESS 0x58 // Address of the single M24M02DRC lockable ID page of the first EEPROM
#define M24M02DRC_2_DATA_ADDRESS 0x54 // Address of the second 1024 page M24M02DRC EEPROM data buffer, 2048 bits (256 8-bit bytes) per page
#define M24M02DRC_2_IDPAGE_ADDRESS 0x5C // Address of the single M24M02DRC lockable ID page of the second EEPROM
/* USER CODE END Includes */


/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);




void i_write(unsigned position, unsigned int data){         //i2c write
	int count=0;
	if(data>255){
		while(data>255){

			data=data-255;
			count++;
		}

	}

	if(count>0){
		//for writing to EEPROM
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
		HAL_Delay(5);

	}else{

		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
	    HAL_Delay(5);

	}
}


unsigned int i_read(unsigned position){                   //i2c read

	int count=0;
	unsigned int data=0;
	//for reading to EEPROM
	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
	data=data+(count*255);
	return data;

}
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

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

//	This command will write the configuration registers of the ltc6811-1s
//	 connected in a daisy chain stack. The configuration is written in descending
//	 order so the last device's configuration is written first.
//
	void ltc6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
	                   uint8_t config[][6] //A two dimensional array of the configuration data that will be written
	                  )
	{
	  const uint8_t BYTES_IN_REG = 6;
	  const uint8_t CMD_LEN = 4+(8*total_ic);
	  uint8_t *cmd;
	  uint16_t cfg_pec;
	  uint8_t cmd_index; //command counter

	  cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));


	  cmd[0] = 0x00;
	  cmd[1] = 0x01;
	  cmd[2] = 0x3d;
	  cmd[3] = 0x6e;


	  cmd_index = 4;
	  for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)       // executes for each ltc6811 in daisy chain, this loops starts with
	  {
	    // the last IC on the stack. The first configuration written is
	    // received by the last IC in the daisy chain

	    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++) // executes for each of the 6 bytes in the CFGR register
	    {
	      // current_byte is the byte counter

	      cmd[cmd_index] = config[current_ic-1][current_byte];            //adding the config data to the array to be sent
	      cmd_index = cmd_index + 1;
	    }

	    cfg_pec = (uint16_t)pec15(BYTES_IN_REG, &config[current_ic-1][0]);   // calculating the PEC for each ICs configuration register data
	    cmd[cmd_index] = (uint8_t)(cfg_pec >> 8);
	    cmd[cmd_index + 1] = (uint8_t)cfg_pec;
	    cmd_index = cmd_index + 2;
	  }


	  wakeup_idle1();                                 //This will guarantee that the ltc6811 isoSPI port is awake.This command can be removed.

	//  output_low(LTC6811_CS);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

	  spi_write_array(CMD_LEN, cmd);
	 // output_high(LTC6811_CS);

	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	  free(cmd);
	}

	//scan I2C adresses
uint8_t retrieveI2Cid(){


	uint8_t i;

for(i=0; i<255; i++){

		if(HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 10) == HAL_OK){

	//HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12);
		break;
		}
		return i;
}

}


	uint8_t spi_read_byte(uint8_t tx_dat)
	{
	    uint8_t data;

	    if ( HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&data, 1, HAL_MAX_DELAY) == HAL_OK )
	    {
	        return(data);
	    }


	    return(-1);
	}
	int osal_DataLength( char *pString ){
	  return (int)( strlen( pString ) );
	}
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
	void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
	                    uint8_t tx_len, //length of the tx data arry
	                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
	                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
	                   ){
		for (uint8_t i = 0; i < tx_len; i++)
		  {
	//	    spi_write(tx_Data[i]);
			 HAL_SPI_Transmit(&hspi1, (uint8_t*)&tx_Data[i], 1, 100);
		  }

		  for (uint8_t i = 0; i < rx_len; i++)
		  {
			  HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)0xFF, (uint8_t*)&rx_data[i], 1, 100);

		  }
	}
	 uint16_t pec15(uint8_t len,uint8_t* data )
		{
		 uint16_t remainder,address;
		remainder = 16;//PEC seed
		for (int i = 0; i < len; i++)
		{
			address = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
			remainder = (remainder << 8 ) ^ crc15Table[address];
		}
		return (remainder*2);//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
		}


	void wakeup_idle1()
	{
	 // output_low(LTC6811_CS);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		spi_read_byte(0xff);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

	  //HAL_Delay(10);
	}
	void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
	                     uint8_t data[] //Array of bytes to be written on the SPI port
	                    )
	{
		uint8_t ret_val;
	    for ( int i = 0; i < len; i++ )
	    {
	        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&data[i], &ret_val, 1, 100);
	    }
	}

	void wakeup_sleep()

	{
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);


	   // cs_low(GPIO_PIN_9);


		   HAL_SPI_Transmit(&hspi1, 0xFF,1, 0xFF);

		   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	  HAL_Delay(1);
	}

	//Read the raw data from the ltc6811 cell voltage register
	void ltc6811_rdcv_reg(uint8_t reg, //Determines which cell voltage register is read back
	                      uint8_t total_ic, //the number of ICs in the
	                      uint8_t data[] //An array of the unparsed cell codes
	                     ){

		 const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
		  uint8_t cmd[4];
		  uint16_t cmd_pec;

		  if (reg == 1)     //1: RDCVA
		  {
		    cmd[1] = 0x04;
		    cmd[0] = 0x00;
		  }
		  else if (reg == 2) //2: RDCVB
		  {
		    cmd[1] = 0x06;
		    cmd[0] = 0x00;
		  }
		  else if (reg == 3) //3: RDCVC
		  {
		    cmd[1] = 0x08;
		    cmd[0] = 0x00;
		  }
		  else if (reg == 4) //4: RDCVD
		  {
		    cmd[1] = 0x0A;
		    cmd[0] = 0x00;
		  }
		  else if (reg == 5) //4: RDCVE
		  {
		    cmd[1] = 0x09;
		    cmd[0] = 0x00;
		  }
		  else if (reg == 6) //4: RDCVF
		  {
		    cmd[1] = 0x0B;
		    cmd[0] = 0x00;
		  }


		  cmd_pec = pec15(2, cmd);
		  cmd[2] = (uint8_t)(cmd_pec >> 8);
		  cmd[3] = (uint8_t)(cmd_pec);

		  wakeup_idle1(); //This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.

		 // output_low(LTC6811_CS);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
		  //uint8_t myData[8] = {1, 1, 1, 1, 1, 1, 1, 1};
		  spi_write_read(cmd,4,data,8);
		  //HAL_SPI_TransmitReceive(&hspi1,cmd,data,8,100);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		 // output_high(LTC6811_CS);


	}

	void ltc6811_adcv(uint8_t MD, //!< ADC Conversion Mode
	                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
	                  uint8_t CH //!< Sets which Cell channels are converted
	                 ){

			uint8_t cmd[4];
			uint16_t cmd_pec;
			uint8_t md_bits;

				md_bits = (MD & 0x02) >> 1;
				cmd[0] = md_bits + 0x02;
				md_bits = (MD & 0x01) << 7;
				cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
				cmd_pec = pec15(2, cmd);

				cmd[2] = (uint8_t)(cmd_pec >> 8);
				cmd[3] = (uint8_t)(cmd_pec);

				wakeup_idle1();
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
				spi_write_array(4, cmd);
				//HAL_SPI_Transmit(&hspi1, cmd, 4,10);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);


	}

	void ltc6811_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
	                       uint8_t total_ic, //The number of ICs in the system
	                       uint8_t *data //Array of the unparsed auxiliary codes
	                      )
	{
	  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
	  uint8_t cmd[4];
	  uint16_t cmd_pec;


	  if (reg == 1)     //Read back auxiliary group A
	  {
	    cmd[1] = 0x0C;
	    cmd[0] = 0x00;
	  }
	  else if (reg == 2)  //Read back auxiliary group B
	  {
	    cmd[1] = 0x0e;
	    cmd[0] = 0x00;
	  }
	  else if (reg == 3)  //Read back auxiliary group B
	  {
	    cmd[1] = 0x0D;
	    cmd[0] = 0x00;
	  }
	  else if (reg == 4)  //Read back auxiliary group B
	  {
	    cmd[1] = 0x0F;
	    cmd[0] = 0x00;
	  }
	  else          //Read back auxiliary group A
	  {
	    cmd[1] = 0x0C;
	    cmd[0] = 0x00;
	  }

	  cmd_pec = pec15(2, cmd);
	  cmd[2] = (uint8_t)(cmd_pec >> 8);
	  cmd[3] = (uint8_t)(cmd_pec);


	  wakeup_idle1(); //This will guarantee that the ltc6811 isoSPI port is awake, this command can be removed.

	  //output_low(LTC6811_CS);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	 // output_high(LTC6811_CS);

	}

	uint32_t ltc6811_pollAdc()
	{
	  uint32_t counter = 0;
	  uint8_t finished = 0;
	  uint8_t current_time = 0;
	  uint8_t cmd[4];
	  uint16_t cmd_pec;


	  cmd[0] = 0x07;
	  cmd[1] = 0x14;
	  cmd_pec = pec15(2, cmd);
	  cmd[2] = (uint8_t)(cmd_pec >> 8);
	  cmd[3] = (uint8_t)(cmd_pec);

	  wakeup_idle1(); //This will guarantee that the ltc6811 isoSPI port is awake. This command can be removed.

	//  output_low(LTC6811_CS);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	  spi_write_array(4,cmd);


	  while ((counter<200000)&&(finished == 0))
	  {

		  uint8_t rx;
		// current_time= HAL_SPI_Receive(&hspi1,rx,0xFF,100);
		 current_time= spi_read_byte(0xFF);
	    if (current_time>0)
	    {
	      finished = 1;
	    }
	    else
	    {
	      counter = counter + 10;
	    }
	  }

	 // output_high(LTC6811_CS);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);


	  return(counter);
	}
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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  int TOT_IC=12;
  int CELL_CH=9;

   uint8_t NUM_RX_BYT = 8;
    uint8_t BYT_IN_REG = 6;
    uint8_t CELL_IN_REG = 3;
   uint8_t NUM_CV_REG = 3;

  uint8_t cell_data[8];
  uint16_t cell_codes[TOT_IC][CELL_CH];
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t pec_error=0;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  //wakeup_idle1();
	  	  ltc6811_adcv(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
	  	  ltc6811_pollAdc();
	  	  //HAL_Delay(3);
	  	  //wakeup_idle1();

	   for(uint8_t cell_reg = 1; cell_reg<NUM_CV_REG+1; cell_reg++){                  //executes once for each of the ltc6811 cell voltage registers

		  uint8_t data_counter = 0;
		  ltc6811_rdcv_reg(cell_reg, TOT_IC, cell_data);

		  	  for(uint8_t current_ic = 0 ; current_ic < TOT_IC; current_ic++){

		   	  	   //Current_ic is used as the IC counter
		   	  	   //Loops once for each of the 3 cell voltage codes in the register

		   	  	   	   for(uint8_t current_cell = 0; current_cell < 3; current_cell++) {

		   	  	   		   //Loops once for each of the 3 cell voltage codes in the register
		   	  	   		   //Each cell code is received as two bytes and is combined to
		   	  	   		   uint16_t parsed_cell = cell_data[data_counter]+(cell_reg-1)+ (cell_data[data_counter + 1] << 8);
		   	  	   		   //Because cell voltage codes are two bytes the data counter
		   	  	   		   cell_codes[current_cell][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
		   	  	   		   //valori in V
		   	  	   		   data_counter = data_counter + 2;
		   	  	   		   uint8_t num[9];
		   	  	   		   sprintf(num, "%d - ", parsed_cell);
		   	  	   		   HAL_UART_Transmit(&huart2, &num, strlen(num), 100);
		   	  	   		   HAL_Delay(100);
		   	  	   	   }
		   	  	   	received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter + 1];
		   	  	   	data_pec = pec15(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
		   	  	   		if(received_pec != data_pec){
		   	  	   			pec_error = -1;
		   	  	   		}
		   	  	   	data_counter = data_counter + 2;
		   	  	   	HAL_UART_Transmit(&huart2, "\r\n", 2, 100);
		  	  }
	   }
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

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
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
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
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
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
