
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

//#include "measurement.h"
//#include "eeprom.h"
//#include "can.h"
#include "ltc68xx.h"
//#include "ltc1865.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


//CAN_FilterConfTypeDef sFilter;
//CanRxMsgTypeDef RxHeader;
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/


const int TOT_IC=8; // number of daisy chain
const int CELL_CH=9;

 uint8_t NUM_RX_BYT = 8;
 uint8_t BYT_IN_REG = 6;
 uint8_t CELL_IN_REG = 3;
 uint8_t NUM_CV_REG = 3;
 uint8_t cell_data[9];
 float cell_voltages[108];
 uint16_t cell_codes_temp[1][9];
 uint16_t parsed_cell;
 uint16_t received_pec;
 uint16_t data_pec;
 uint8_t pec_error=0;
 int counterCicle = 0;
 float *av_temp1 = 0;
 float *av_temp2 = 0;
 float *av_temp3 = 0;
 uint32_t *adcBuffer;

 uint8_t *totalPack;
  		 uint8_t *tractiveVoltage;

  		 uint16_t *max_vol;
  			 uint16_t *min_vol;
  			 float *average_vol;

  			uint16_t *temp = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

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
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* SETTING CAN */
//  sFilter.FilterMode = CAN_FILTERMODE_IDMASK;
//  sFilter.FilterMode = CAN_FILTERMODE_IDMASK;
//  sFilter.FilterIdLow = 0;
//  sFilter.FilterIdHigh = 0;
//  sFilter.FilterMaskIdHigh = 0;
//  sFilter.FilterMaskIdLow = 0;
//  sFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
//  sFilter.BankNumber = 0;
//  sFilter.FilterScale = CAN_FILTERSCALE_16BIT;
//  sFilter.FilterActivation = ENABLE;
//  HAL_CAN_ConfigFilter(&hcan, &sFilter);
//
//  HAL_CAN_Init(&hcan);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //float offset=Current_Calibration_Offset(hadc1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t voltages[9];
 // voltages = (uint16_t*)malloc(9*sizeof(uint16_t));
//  cell_data = (uint8_t*)malloc(9*sizeof(uint8_t));
//  cell_codes = (uint16_t*)malloc(9*12*sizeof(uint16_t));


//  float offset=Current_Calibration_Offset(hadc1);
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);


	  	int id = 0;

	 	 // GPIO Logic
	 	 //Precharge off
	 	// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//	 	 GPIO_PinState ShutDown_Status;
//	 	 ShutDown_Status = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
//	 	 if(ShutDown_Status == GPIO_PIN_SET){
//
//	 		 while(totalPack != tractiveVoltage){
//	 			 menu_1_read_single_ended(hspi1, tractiveVoltage, totalPack);
//	 		 }
//	 		 // Monitorare Total voltage e tractive system voltage
//	 		 // Quando sono uguali ---> precharge on
//	 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
//	 		 HAL_Delay(1);
//	 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//	 	 }
//
//	 	 if(ShutDown_Status == GPIO_PIN_RESET){
//	 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
//	 		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//	 		 //CAN SEND TS OFF
//	 	 }

	 	 //QUANDO ARRIVA IL MESSAGGIO CAN TS ON ----> ATTIVA IL TRACTIVE SYSTEM

	 	 //QUANDO ARRIVA IL MESSAGGIO CAN TS OFF ----> DISATTIVA IL TRACTIVE SYSTEM
//
//	 	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer, 1); // Call DMA for ADC1
//
//	 	HAL_Delay(0.05); // Wait for conversion
//	 	HAL_ADC_Stop_DMA(&hadc1);
//	 	uint32_t Current = Get_Amps_Value(adcBuffer,(uint16_t)offset);
//
//


	  	 /* ----- Voltages ------*/
	  	 ltc6804_adcv(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
	  	 HAL_Delay(10);
		 uint8_t data_counter = 0;
		 for(uint8_t current_ic = 7 ; current_ic < TOT_IC; current_ic++){

				 ltc6804_rdcv_reg(current_ic, TOT_IC, cell_data, hspi1, huart2);

		 	 	 array_voltages(voltages, cell_data);



		 	 	char num[2];
		 	 	sprintf(num, "\n");
		 	 	HAL_UART_Transmit(&huart2, &num, strlen(num), 100);
		 	 	 for(int i = 0; i < 9; i++){
		 	 		 char v[32];
		 	 		 sprintf(v, "%d - ",voltages[i]);
		 	 		 HAL_UART_Transmit(&huart2, &v, strlen(v), 100);
		 	 		 cell_voltages[current_ic*9+i] = voltages[i]*0.0001f;
		 	  	 }



		 }
		 //char num[8];

//		 max_min_voltages(cell_codes, max_vol, min_vol, average_vol);
//		 //Can Messages
//
//		 /* ----- Temperatures -----*/
//
//		 //odd temp
		 HAL_Delay(1000);
		 ltc6804_address_temp_odd(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
		 HAL_Delay(1000);
		 ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
		 HAL_Delay(10);
		 for(uint8_t current_ic = 7 ; current_ic < TOT_IC; current_ic++){
			 ltc6804_rdcv_temp(current_ic, TOT_IC, cell_data, hspi1,huart2);
			 	 array_temp_odd(temp, cell_data);

			 	 cell_codes_temp[current_ic][0] = temp[0];
			 	 cell_codes_temp[current_ic][2] = temp[2];
			 	 cell_codes_temp[current_ic][4] = temp[4];
			 	 cell_codes_temp[current_ic][6] = temp[6];
			 	 cell_codes_temp[current_ic][8] = temp[8];

		 }

		 //
//		 //ltc6804_rdcv_temp(...);
//		 convert_temp();
//
//		 //even temp
		 HAL_Delay(1000);
		 ltc6804_address_temp_even(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
		 HAL_Delay(1000);
		 ltc6804_adcv_temp(MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL, hspi1);
		 HAL_Delay(10);
		 for(uint8_t current_ic = 7 ; current_ic < TOT_IC; current_ic++){
			 ltc6804_rdcv_temp(current_ic, TOT_IC, cell_data, hspi1, huart2);
		 		 array_temp_even(temp, cell_data);

		 		 cell_codes_temp[current_ic][1] = temp[1];
		 		 cell_codes_temp[current_ic][3] = temp[3];
		 		 cell_codes_temp[current_ic][5] = temp[5];
		 		 cell_codes_temp[current_ic][7] = temp[7];

		 }
		 for(int i = 0; i < 9; i++){
				 char v[32];
				 sprintf(v, "%x - ",cell_codes_temp[7][i]);
				 HAL_UART_Transmit(&huart2, &v, strlen(v), 100);
				 }
//		 // Controllo Temperatura massima
//		 uint16_t *max_temp = 0;
//		 if(counterCicle % 3 == 0){
//			 max_ave_temp(cell_codes_temp, max_temp, av_temp1);
//		 }
//		 if(counterCicle % 3 == 1){
//		 	 max_ave_temp(cell_codes_temp, max_temp, av_temp2);
//		 }
//		 if(counterCicle % 3 == 2){
//		 	 max_ave_temp(cell_codes_temp, max_temp, av_temp3);
//		 }
//		 float average = 0;
//		 if(counterCicle > 2){
//
//		 average = (*av_temp1 + *av_temp2 + *av_temp3)/3;
//
//
//
//		 if(average < 60 && *max_vol*0.0001f < 4.20 && *min_vol*0.0001f > 2.80){
//			 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
//		 }
//		 }
//		 float **converted_temp;
//		 for(int i = 0; i < 12; i++){
//			 for(int j = 0; j < 9; j++){
//				 converted_temp[i][j] = convert_temp(cell_codes_temp[i][j]);
//			 }
//		 }
//
//		 //CAN MESSAGES
//		 uint16_t tot_vol = total_pack_voltage(cell_codes);
////
////		 CAN_Send(id,(uint8_t)max_vol, 8, hcan);
////		 CAN_Send(id,(uint8_t)min_vol, 8, hcan);
////		 CAN_Send(id, tot_vol, 8, hcan);
////		 CAN_Send(id, (tot_vol << 8), 8, hcan);
////		 CAN_Send(id,*max_temp, 8, hcan);
////		 CAN_Send(id,*max_temp << 8, 8, hcan);
////		 CAN_Send(id,(uint8_t)average, 8, hcan);
////		 for(int i = 0; i < TOT_IC; i++){
////			 for(int j = 0; j < 9; j++){
////				 CAN_Send(id, (uint8_t)cell_codes[i][j], 8, hcan);
////				 CAN_Send(id, (uint8_t)cell_codes[i][j] << 8, 8, hcan);
////				 CAN_Send(id, (uint8_t)cell_codes_temp[i][j], 8, hcan);
////				 CAN_Send(id, (uint8_t)cell_codes_temp[i][j] << 8, 8, hcan);
////			 }
////		 }
//

		 counterCicle = counterCicle + 1;
		 HAL_Delay(500);
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
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
  sConfig.Channel = ADC_CHANNEL_12;
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
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SJW = CAN_SJW_1TQ;
  hcan.Init.BS1 = CAN_BS1_1TQ;
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
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

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  HAL_GPIO_WritePin(GPIOA, PrechargeEnded_Pin|CS_6820_Pin|CS_SDCARD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EEPromWc_Pin|GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin : CurrentSensor_Pin */
  GPIO_InitStruct.Pin = CurrentSensor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CurrentSensor_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PrechargeEnded_Pin CS_6820_Pin CS_SDCARD_Pin */
  GPIO_InitStruct.Pin = PrechargeEnded_Pin|CS_6820_Pin|CS_SDCARD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EEPromWc_Pin PB3 */
  GPIO_InitStruct.Pin = EEPromWc_Pin|GPIO_PIN_3;
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
