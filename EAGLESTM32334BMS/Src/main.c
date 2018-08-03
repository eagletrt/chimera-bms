
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

#include "ltc_68xx.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#define TOT_IC 12 // number of daisy chain
#define CELL_CH 9
CAN_FilterConfTypeDef tsONfilter;
CanRxMsgTypeDef RxMsg;
CanTxMsgTypeDef TxMsg;
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

uint8_t cell_data[9];
uint16_t cell_voltages[108][2];
uint16_t cell_temp[108][2];
int counterCicle = 0;
uint16_t temp[9];
uint16_t voltages[9];
uint16_t data_pec;
uint8_t pec_error=0;
uint8_t error = 0;
uint32_t pack_v = 0;

uint32_t temp_v = 0;
uint16_t maxtemp = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int GetMSB(int intValue)
{
   return (intValue & 0xFFFF0000);
}
int GetLSB(int intValue)
{
  return (intValue & 0x0000FFFF);
}
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
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  //CAN FIlter Initialization
  tsONfilter.FilterMode = CAN_FILTERMODE_IDLIST;
  tsONfilter.FilterIdLow = 0x55 << 5;
  tsONfilter.FilterIdHigh = 0x55 << 5;
  tsONfilter.FilterMaskIdHigh = 0x55 << 5;
  tsONfilter.FilterMaskIdLow = 0x55 << 5;
  tsONfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  tsONfilter.BankNumber = 0;
  tsONfilter.FilterScale  = CAN_FILTERSCALE_16BIT;
  tsONfilter.FilterActivation = ENABLE;
  HAL_CAN_ConfigFilter(&hcan, &tsONfilter);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

  //First cell voltages read
  ltc6804_adcv(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
  HAL_Delay(10);
  for(uint8_t current_ic = 0; current_ic < TOT_IC; current_ic++){
	  error += ltc6804_rdcv_voltages(current_ic, hspi1, cell_voltages);
  }
  cell_voltages[90][0]=(cell_voltages[89][0]+cell_voltages[88][0])/2;
  cell_voltages[91][0]=(cell_voltages[92][0]+cell_voltages[93][0])/2;
  cell_voltages[90][1]=0;
  cell_voltages[91][1]=0;

  //First cell voltages check
  uint8_t fault = 0;
  for(int i = 0; i < 108; i++){

	  if(cell_voltages[i][0] < 28000 || cell_voltages[i][1] > 1000 || cell_voltages[i][0] > 42000){

		  fault++;
	  }

  }
  if(error == 0){
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
  }else{

	  TxMsg.IDE = CAN_ID_STD;
	  TxMsg.RTR = CAN_RTR_DATA;
	  TxMsg.DLC = 8;
	  TxMsg.StdId = 0xAA;
	  TxMsg.Data[0] = 0x08;
	  TxMsg.Data[1] = 0x01;
	  TxMsg.Data[2] = 0x00;
	  TxMsg.Data[3] = 0x00;
	  TxMsg.Data[4] = 0x00;
	  TxMsg.Data[5] = 0x00;
	  TxMsg.Data[6] = 0x00;
	  TxMsg.Data[7] = 0x00;
	  hcan.pTxMsg = &TxMsg;
	  HAL_CAN_Transmit(&hcan, 10);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

	  /* ----- Voltages ------*/
	  ltc6804_adcv(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  HAL_Delay(10);
	  for(uint8_t current_ic = 0; current_ic < TOT_IC; current_ic++){

		  error += ltc6804_rdcv_voltages(current_ic, hspi1, cell_voltages);

	  }
	  cell_voltages[90][0]=(cell_voltages[89][0]+cell_voltages[88][0])/2;
	  cell_voltages[91][0]=(cell_voltages[92][0]+cell_voltages[93][0])/2;
	  cell_voltages[90][1]=0;
	  cell_voltages[91][1]=0;
	  pack_v = 0;
	  for(int i = 0; i < 108; i++){

		  if(cell_voltages[i][0] < 28000 || cell_voltages[i][1] > 1000 || cell_voltages[i][0] > 42000){
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

		  TxMsg.IDE = CAN_ID_STD;
		  TxMsg.RTR = CAN_RTR_DATA;
		  TxMsg.DLC = 8;
		  TxMsg.StdId = 0xAA;
		  TxMsg.Data[0] = 0x08;
		  TxMsg.Data[1] = 0x02;
		  TxMsg.Data[2] = 0x00;
		  TxMsg.Data[3] = 0x00;
		  TxMsg.Data[4] = 0x00;
		  TxMsg.Data[5] = 0x00;
		  TxMsg.Data[6] = 0x00;
		  TxMsg.Data[7] = 0x00;
		  hcan.pTxMsg = &TxMsg;
		  HAL_CAN_Transmit(&hcan, 10);
		  }
		  pack_v += cell_voltages[i][0];
	  }
	  	  //		 /* ----- Temperatures -----*/
	  	  //
//	  	  uint16_t temp0, temp1, temp2, temp3;
//	  	  temp0 = 0;
//	  	  temp1 = 0;
//	  	  temp2 = 0;
//	  	  temp3 = 0;
	  	  ltc6804_address_temp_odd(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  	  // HAL_Delay(10);
	  	  ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  	  HAL_Delay(10);
	  	  for(uint8_t current_ic = 0; current_ic < TOT_IC; current_ic++){
	  	  ltc6804_rdcv_temp(current_ic, 1, cell_temp, hspi1);
	  	  }
	  	  //HAL_Delay(500);
//	  	  array_temp_odd(temp, cell_data);
//	  	  		 				 	if(current_ic == 0){
//	  	  		 				 		temp0 = (uint16_t)(convert_temp(temp[0])*100);
//	  	  		 				 		temp2 = (uint16_t)(convert_temp(temp[2])*100);
//	  	  		 				 	}
//	  	  		 				 	 if(current_ic != 0){
//	  	  		 				 		 cell_temps[current_ic*9+0] = (uint16_t)(convert_temp(temp[0])*100);
//	  	  		 				 		 cell_temps[current_ic*9+2] = (uint16_t)(convert_temp(temp[2])*100);
//	  	  		 				 	 }
//
//	  	  		 				 	 cell_temps[current_ic*9+4] = (uint16_t)(convert_temp(temp[4])*100);
//	  	  		 				 	 cell_temps[current_ic*9+6] = (uint16_t)(convert_temp(temp[6])*100);
//	  	  		 				 	 cell_temps[current_ic*9+8] = (uint16_t)(convert_temp(temp[8])*100);


	  	  //		 			 cell_temps[0] = temp0;
	  	  //		 			 cell_temps[2] = temp2;
	  	  		 //
	  	  	//		 //ltc6804_rdcv_temp(...);
	  	  	//		 convert_temp();
	  	  	//
	  	  	//		 //even temp
	  	  			 ltc6804_address_temp_even(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  	  			 //HAL_Delay(10);
	  	  			 ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  	  			 HAL_Delay(10);
	  	  			 for(uint8_t current_ic = 0; current_ic < TOT_IC; current_ic++){
	  	  				 ltc6804_rdcv_temp(current_ic, 0, cell_temp, hspi1);
	  	  			 }
	  	  				 //HAL_Delay(500);
//	  	  				 array_temp_even(temp, cell_data);
//	  	  				 if(current_ic == 0){
//	  	  				 	 temp1 = (uint16_t)(convert_temp(temp[1])*100);
//	  	  				 	 temp3 = (uint16_t)(convert_temp(temp[3])*100);
//	  	  				 }
//	  	  				 	 if(current_ic != 0){
//	  	  				 		 cell_temps[current_ic*9+1] = (uint16_t)(convert_temp(temp[1])*100);
//	  	  				 		 cell_temps[current_ic*9+3] = (uint16_t)(convert_temp(temp[3])*100);
//	  	  				 	 }
//
//	  	  //			 		 cell_temps[current_ic*9+1] = (uint16_t)(convert_temp(temp[1])*100);
//	  	  //			 		 cell_temps[current_ic*9+3] = (uint16_t)(convert_temp(temp[3])*100);
//	  	  			 		 cell_temps[current_ic*9+5] = (uint16_t)(convert_temp(temp[5])*100);
//	  	  			 		 cell_temps[current_ic*9+7] = (uint16_t)(convert_temp(temp[7])*100);
//	  	  //			 		for(int i = 0; i < 9; i++){
//	  	  //			 		 char v[32];
//	  	  //			 					  					 sprintf(v, "%d - ",temp[i]);
//	  	  //			 					 					 HAL_UART_Transmit(&huart2, &v, strlen(v), 100);
//	  	  //			 		}
//	  	  			 }
//	  	  			 cell_temps[0] = temp0;
//	  	  			 cell_temps[2] = temp2;
//	  	  			 cell_temps[1] = temp1;
//	  	  			 cell_temps[3] = temp3;







	  ltc6804_stop_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);

	  temp_v=0;
	  maxtemp =0;
	  for(int i = 0; i < 108; i++){

		  if(cell_temp[i][0] > 80000 || cell_temp[i][1] > 1000){
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

			  TxMsg.IDE = CAN_ID_STD;
			  TxMsg.RTR = CAN_RTR_DATA;
			  TxMsg.DLC = 8;
			  TxMsg.StdId = 0xAA;
			  TxMsg.Data[0] = 0x08;
			  TxMsg.Data[1] = 0x03;
			  TxMsg.Data[2] = 0x00;
			  TxMsg.Data[3] = 0x00;
			  TxMsg.Data[4] = 0x00;
			  TxMsg.Data[5] = 0x00;
			  TxMsg.Data[6] = 0x00;
			  TxMsg.Data[7] = 0x00;
			  hcan.pTxMsg = &TxMsg;
			  HAL_CAN_Transmit(&hcan, 10);

		  }
		  if(cell_temp[i][0] > maxtemp){
			  maxtemp = cell_temp[i][0];
		  }
		  temp_v += cell_temp[i][0];
	  }
	  temp_v = (uint32_t)((float)(temp_v / 108));
	  if(temp_v > 65000){
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

		  TxMsg.IDE = CAN_ID_STD;
		  TxMsg.RTR = CAN_RTR_DATA;
		  TxMsg.DLC = 8;
		  TxMsg.StdId = 0xAA;
		  TxMsg.Data[0] = 0x08;
		  TxMsg.Data[1] = 0x04;
		  TxMsg.Data[2] = 0x00;
		  TxMsg.Data[3] = 0x00;
		  TxMsg.Data[4] = 0x00;
		  TxMsg.Data[5] = 0x00;
		  TxMsg.Data[6] = 0x00;
		  TxMsg.Data[7] = 0x00;
		  hcan.pTxMsg = &TxMsg;
		  HAL_CAN_Transmit(&hcan, 10);
	  }

	  TxMsg.IDE = CAN_ID_STD;
	  TxMsg.RTR = CAN_RTR_DATA;
	  TxMsg.DLC = 8;
	  TxMsg.StdId = 0xAA;
	  TxMsg.Data[0] = 0x01;
	  TxMsg.Data[1] = (uint8_t) (pack_v >> 16);
	  TxMsg.Data[2] = (uint8_t) (pack_v >> 8);
	  TxMsg.Data[3] = (uint8_t) (pack_v);
	  TxMsg.Data[4] = (uint8_t) (temp_v >> 8);
	  TxMsg.Data[5] = (uint8_t) (temp_v);
	  TxMsg.Data[6] = (uint8_t) (maxtemp >> 8);
	  TxMsg.Data[7] = (uint8_t) (maxtemp);
	  hcan.pTxMsg = &TxMsg;
	  HAL_CAN_Transmit(&hcan, 10);

	  hcan.pRxMsg = &RxMsg;
	  if(HAL_CAN_Receive(&hcan,CAN_FIFO0, 1) == HAL_OK){

		  if(RxMsg.StdId == 0x55 && RxMsg.Data[0] == 0x0A){

			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
			  HAL_Delay(15000);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
			  HAL_Delay(1);
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
			  //HAL_Delay(2000);
			  TxMsg.IDE = CAN_ID_STD;
			  TxMsg.RTR = CAN_RTR_DATA;
			  TxMsg.DLC = 8;
			  TxMsg.StdId = 0xAA;
			  TxMsg.Data[0] = 0x03;
			  TxMsg.Data[1] = 0x00;
			  TxMsg.Data[2] = 0x00;
			  TxMsg.Data[3] = 0x00;
			  TxMsg.Data[4] = 0x00;
			  TxMsg.Data[5] = 0x00;
			  TxMsg.Data[6] = 0x00;
			  TxMsg.Data[7] = 0x00;
			  hcan.pTxMsg = &TxMsg;
			  HAL_CAN_Transmit(&hcan, 10);
		  }
		  else if(RxMsg.StdId == 0x55 && RxMsg.Data[0] == 0x0B){
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

			  TxMsg.IDE = CAN_ID_STD;
			  TxMsg.RTR = CAN_RTR_DATA;
			  TxMsg.DLC = 8;
			  TxMsg.StdId = 0xAA;
			  TxMsg.Data[0] = 0x04;
			  TxMsg.Data[1] = 0x00;
			  TxMsg.Data[2] = 0x00;
			  TxMsg.Data[3] = 0x00;
			  TxMsg.Data[4] = 0x00;
			  TxMsg.Data[5] = 0x00;
			  TxMsg.Data[6] = 0x00;
			  TxMsg.Data[7] = 0x00;
			  hcan.pTxMsg = &TxMsg;
			  HAL_CAN_Transmit(&hcan, 10);
		  }

	  }

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */


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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
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

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
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
  HAL_GPIO_WritePin(GPIOA, PreChargeEnd_Pin|TS_ON_Pin|BMS_FAULT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_6820_GPIO_Port, CS_6820_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PreChargeEnd_Pin CS_6820_Pin TS_ON_Pin BMS_FAULT_Pin */
  GPIO_InitStruct.Pin = PreChargeEnd_Pin|CS_6820_Pin|TS_ON_Pin|BMS_FAULT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ShutDownStatus_Pin */
  GPIO_InitStruct.Pin = ShutDownStatus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ShutDownStatus_GPIO_Port, &GPIO_InitStruct);

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
