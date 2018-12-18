/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */

#include "main.h"

#include "stm32f3xx_hal.h"
#include "ltc_68xx.h"
#include "can.h"

const uint8_t InvBusVoltage[] = {0x3D, 0xEB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t TsON[] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t TsOFF[] = {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM6_Init(void);

CAN_FilterConfTypeDef runFilter;
CAN_FilterConfTypeDef pcFilter;
CanRxMsgTypeDef RxMsg;
CanTxMsgTypeDef TxMsg;

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

Cell cells[N_CELLS];

Pack pack;

// TODO: remove old vars
uint16_t cell_voltages[108][2];
uint16_t cell_voltages_vector[108];
uint8_t parity = 0;
uint16_t cell_temperatures[108][2];
uint16_t cell_temperatures_vector[108];
uint32_t pack_v;
uint16_t pack_t;
uint16_t max_t;
uint8_t cell;
uint16_t value;

uint8_t data[8];

uint32_t adcCurrent[512];
int32_t instCurrent;
int32_t current;
int32_t current_s;

uint8_t fault_counter = 0;
uint8_t BMS_status = 0;
uint8_t precharge;
uint16_t precharge_timer = 0;
int32_t bus_voltage;

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SPI1_Init();
	MX_ADC1_Init();
	MX_CAN_Init();
	MX_TIM6_Init();

	cells_init(cells);

	// CAN FIlter Initialization
	runFilter.FilterNumber = 0;
	runFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	runFilter.FilterIdLow = 0x55 << 5;
	runFilter.FilterIdHigh = 0xA8 << 5;
	runFilter.FilterMaskIdHigh = 0x55 << 5;
	runFilter.FilterMaskIdLow = 0x55 << 5;
	runFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	runFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	runFilter.FilterActivation = ENABLE;
	HAL_CAN_ConfigFilter(&hcan, &runFilter);

	pcFilter.FilterNumber = 0;
	pcFilter.FilterMode = CAN_FILTERMODE_IDLIST;
	pcFilter.FilterIdLow = 0x181 << 5;
	pcFilter.FilterIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdHigh = 0x181 << 5;
	pcFilter.FilterMaskIdLow = 0x181 << 5;
	pcFilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	pcFilter.FilterScale = CAN_FILTERSCALE_16BIT;
	pcFilter.FilterActivation = ENABLE;

	// BMS Status OFF
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	precharge = 1;

	// Start current measuring
	HAL_ADC_Start_DMA(&hadc1, adcCurrent, 512);

	while (1) {
		if (HAL_CAN_GetState(&hcan) == HAL_CAN_ERROR_BOF) {
			// BMS Status OFF
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

		}

		// Voltages
		ltc6804_rdcv_voltages(cells, &hspi1);

		// Temperatures
		ltc6804_rdcv_temp(cells, &hspi1);

		//Cells 90 and 91 not working
		cells[90].voltage = (cells[89].voltage + cells[88].voltage) / 2;
		cells[90].temperature = (cells[89].temperature + cells[88].temperature)	/ 2;
		cells[90].voltage_faults = 0;
		cells[90].temperature_faults = 0;

		cells[91].voltage = (cells[92].voltage + cells[93].voltage) / 2;
		cells[91].temperature = (cells[92].temperature + cells[93].temperature)	/ 2;
		cells[91].voltage_faults = 0;
		cells[91].temperature_faults = 0;

		//Current
		instCurrent = 0;
		for (int i = 0; i < 512; i++)
			instCurrent += adcCurrent[i];

		instCurrent = -(instCurrent / 512 - 2595) * 12.91;
		current_s = current_s + instCurrent - (current_s / 16);
		pack.current = current_s / 16;

		status(cells, &pack);

		if (pack.state == PACK_ERROR) { // Cell error

			fault_counter++;
			if (fault_counter > 15) {

				//Set the BMS to fault
				BMS_status = 0;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
				uint8_t i;

				for (i = 0; i < N_CELLS; i++) {
					//Sends the error message indicating the fault and the TS OFF
					CellErr(&hcan, i, cells[i]);
				}
			}
		} else {

			fault_counter = 0;
			if (BMS_status == 0) {

				HAL_Delay(10);
				BMS_status = 1;
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);

			}

		}

		// Send pack data via CAN
		data[0] = 0x01;
		data[1] = (uint8_t) (pack.voltage >> 16);
		data[2] = (uint8_t) (pack.voltage >> 8);
		data[3] = (uint8_t) (pack.voltage);
		data[4] = (uint8_t) (pack.temperature >> 8);
		data[5] = (uint8_t) (pack.temperature);
		data[6] = (uint8_t) (max_t >> 8);
		data[7] = (uint8_t) (max_t);
		CAN_Transmit(&hcan, 0xAA, 8, data);

		// Send current data via CAN
		data[0] = 0x05;
		data[1] = (int8_t) (current >> 16);
		data[2] = (int8_t) (current >> 8);
		data[3] = (int8_t) current;
		data[4] = 0;
		data[5] = 0;
		data[6] = 0;
		data[7] = 0;
		CAN_Transmit(&hcan, 0xAA, 8, data);

		if (precharge == 0) {

			//inverter1 bus voltage request
			CAN_Transmit(&hcan, 0x201, 3, InvBusVoltage);

			for (int i = 0; i < 2; i++) {

				hcan.pRxMsg = &RxMsg;
				if (HAL_CAN_Receive(&hcan, CAN_FIFO0, 1) == HAL_OK) {

					if (RxMsg.StdId == 0x181 && RxMsg.Data[0] == 0xEB) {

						bus_voltage = RxMsg.Data[2] << 8;
						bus_voltage += RxMsg.Data[1];

					}

				}
			}
			bus_voltage = bus_voltage * 10000 / 31.499;

			if (bus_voltage > pack_v * 0.90) {
				HAL_Delay(1000);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
				HAL_Delay(1);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				precharge = 1;
				HAL_CAN_ConfigFilter(&hcan, &runFilter);
				CAN_Transmit(&hcan, 0xAA, 8, TsON);

			}
			if (++precharge_timer > 450) {

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
				precharge = 1;
				HAL_CAN_ConfigFilter(&hcan, &runFilter);
				// CAN MESS ERR
			}

		}

		hcan.pRxMsg = &RxMsg;
		if (HAL_CAN_Receive(&hcan, CAN_FIFO0, 1) == HAL_OK) {

			if (RxMsg.StdId == 0x55 && RxMsg.Data[0] == 0x0A
					&& RxMsg.Data[1] == 0x00) {

				//TS ON procedure with inverter
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				precharge = 0;
				precharge_timer = 0;
				bus_voltage = 0;
				HAL_CAN_ConfigFilter(&hcan, &pcFilter);

			} else if (RxMsg.StdId == 0x55 && RxMsg.Data[0] == 0x0A
					&& RxMsg.Data[1] == 0x01) {

				//TS ON procedure with delay as pre-charge control
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
				HAL_Delay(15000);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
				HAL_Delay(1);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				CAN_Transmit(&hcan, 0xAA, 8, TsON);

			} else if (RxMsg.StdId == 0x55 && RxMsg.Data[0] == 0x0B) {

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
				precharge = 1;
				CAN_Transmit(&hcan, 0xAA, 8, TsOFF);

			} else if (RxMsg.StdId == 0xA8) {

				for (uint8_t i = 0; i < 108; i += 3) {

					data[0] = i;
					data[1] = (uint8_t) (cell_voltages[i][0] / 400);   //*0.04
					data[2] = (uint8_t) (cell_temperatures[i][0] / 40);   //*.4
					data[3] = (uint8_t) (cell_voltages[i + 1][0] / 400);
					data[4] = (uint8_t) (cell_temperatures[i + 1][0] / 40);
					data[5] = (uint8_t) (cell_voltages[i + 2][0] / 400);
					data[6] = (uint8_t) (cell_temperatures[i + 2][0] / 40);
					data[7] = 0;
					CAN_Transmit(&hcan, 0xAB, 8, data);
					HAL_Delay(10);
				}

			}
		}
	}
}

void cells_init(Cell cells[]) {

	uint8_t i;
	for (i = 0; i < N_CELLS; i++) {
		cells[i].voltage = 0;
		cells[i].temperature = 0;
		cells[i].voltage_faults = 0;
		cells[i].temperature_faults = 0;
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void) {

	ADC_MultiModeTypeDef multimode;
	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* CAN init function */
static void MX_CAN_Init(void) {

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
	if (HAL_CAN_Init(&hcan) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI1 init function */
static void MX_SPI1_Init(void) {

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
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM6 init function */
static void MX_TIM6_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig;

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 32000;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 0;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE()
	;

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, PreChargeEnd_Pin | TS_ON_Pin | BMS_FAULT_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CS_6820_GPIO_Port, CS_6820_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : PreChargeEnd_Pin CS_6820_Pin TS_ON_Pin BMS_FAULT_Pin */
	GPIO_InitStruct.Pin = PreChargeEnd_Pin | CS_6820_Pin | TS_ON_Pin
			| BMS_FAULT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : ShutDownStatus_Pin */
	GPIO_InitStruct.Pin = ShutDownStatus_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(ShutDownStatus_GPIO_Port, &GPIO_InitStruct);

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
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
