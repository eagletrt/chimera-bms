/**
 * @file	main.c
 * @brief	Main program body
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
*/

#include "main.h"

#include "stm32f3xx_hal.h"
#include "chimera_config.h"
#include "pack.h"
#include "can.h"
#include "error.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM6_Init(void);
static void MX_SPI1_Init(void);

CanRxMsgTypeDef can_rx;

DMA_HandleTypeDef hdma_adc1;
ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi1;

CAN_HandleTypeDef hcan;

TIM_HandleTypeDef htim6;

PACK_T pack;

ERROR_STATUS_T can_error;
ERROR_T error = ERROR_OK;

uint8_t data[8];

BMS_STATUS_T state = BMS_INIT;
uint32_t precharge_timer;
int32_t bus_voltage;

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_CAN_Init();
	MX_TIM6_Init();
	MX_SPI1_Init();

	can_init(&hcan);
	error_init(&can_error);

	pack_init(&hadc1, &pack);

	// BMS Status OFF
	HAL_GPIO_WritePin(GPIOA, PIN_BMS_FAULT, GPIO_PIN_RESET);

	state = BMS_OFF;
	while (1)
	{
		if (can_check_error(&hcan))
		{
			error_set(ERROR_CAN, &can_error, HAL_GetTick());
		}
		else
		{
			error_unset(ERROR_CAN, &can_error);
		}

		error = ERROR_OK;
		error_check_fatal(&can_error, HAL_GetTick(), &error);
		if (error != ERROR_OK)
		{
			break;
		}

		can_receive(&hcan, &can_rx);

		if (can_rx.StdId == CAN_CTRL_ID && can_rx.Data[0] == CAN_CTRL_TS_OFF)
		{
			// TS Off
			HAL_GPIO_WritePin(GPIOA, PIN_TS_ON, GPIO_PIN_RESET);
			state = BMS_OFF;

			can_send_ts_off(&hcan);
		}

		else if (can_rx.StdId == 0xA8)
		{ // "Initial check". TODO: When is it called?
			uint8_t i;

			for (i = 0; i < PACK_MODULE_COUNT; i += 3)
			{
				data[0] = i;
				data[1] = (uint8_t) (pack.voltages[i].value / 400);   //*0.04
				data[2] = (uint8_t) (pack.temperatures[i].value / 40);   //*.4
				data[3] = (uint8_t) (pack.voltages[i + 1].value / 400);
				data[4] = (uint8_t) (pack.temperatures[i + 1].value / 40);
				data[5] = (uint8_t) (pack.voltages[i + 2].value / 400);
				data[6] = (uint8_t) (pack.temperatures[i + 2].value / 40);
				data[7] = 0;
				can_send(&hcan, 0xAB, 8, data);
				HAL_Delay(10);
			}
		}

		// Voltages
		error = ERROR_OK;
		pack_update_voltages(&hspi1, pack.voltages, &error);

		if (error != ERROR_OK)
		{
			break;
		}

		// Temperatures
		error = ERROR_OK;
		pack_update_temperatures(&hspi1, pack.temperatures, &error);

		if (error != ERROR_OK)
		{
			break;
		}

		// Current
		error = ERROR_OK;
		pack_update_current(&pack.current, &error);

		if (error != ERROR_OK)
		{
			break;
		}

		// Update total values
		pack_update_status(&pack);

		can_send_pack_state(&hcan, pack);
		can_send_current(&hcan, pack.current.value);

		switch (state)
		{
		case BMS_PRECHARGE:
			precharge_check();
			break;
		case BMS_OFF:
			if (can_rx.StdId == CAN_CTRL_ID)
			{
				if (can_rx.Data[0] == CAN_CTRL_TS_ON)
				{
					// TS On

					if (can_rx.Data[1] == 0x00)
					{
						// Precharge
						precharge_start();

						precharge_timer = HAL_GetTick();
						bus_voltage = 0;

						can_filter_precharge(&hcan);
					}
					else if (can_rx.Data[1] == 0x01)
					{ 	// TODO: Undocumented
						// Direct TS ON
						precharge_start();

						HAL_Delay(15000); // TODO: Use a timer
						precharge_end();
					}
				}
			}

			break;
		case BMS_ON:
			break;
		}
	}

	// In case of fatal error

	// Set the BMS to fault
	state = BMS_HALT;
	HAL_GPIO_WritePin(GPIOA, PIN_BMS_FAULT, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, PIN_TS_ON, GPIO_PIN_RESET);

	can_send_error(&hcan, error, &pack);

	while (1)
	{
		// TODO: add some command to restart the bms by breaking this cycle
	}

	return 0;
}

void precharge_start()
{
	HAL_GPIO_WritePin(GPIOA, PIN_TS_ON, GPIO_PIN_SET);
	state = BMS_PRECHARGE;
}

void precharge_end()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

	can_filter_normal(&hcan);
	can_send_ts_on(&hcan);

	state = BMS_ON;
}

void precharge_check()
{
	//inverter1 bus voltage request
	can_request_inverter_voltage(&hcan);

	for (uint8_t i = 0; i < 2; i++)
	{ // Why this cycle?

		if (can_receive(&hcan, &can_rx))
		{
			// check bus voltage from inverter
			if (can_rx.StdId == 0x181 && can_rx.Data[0] == 0xEB)
			{

				bus_voltage = can_rx.Data[2] << 8;
				bus_voltage += can_rx.Data[1];
			}

		}
	}

	bus_voltage = bus_voltage * 10000 / 31.499;

	if (bus_voltage > pack.total_voltage * 0.90)
	{
		HAL_Delay(1000); // TODO: use a timer

		precharge_end();
	}

	if (HAL_GetTick() - precharge_timer > 450)
	{ // Timeout

		HAL_GPIO_WritePin(GPIOA, PIN_TS_ON, GPIO_PIN_RESET);

		//error_set(ERROR_PRECHARGE_ERROR, HAL_GetTick());

		can_filter_normal(&hcan);

		// CAN MESS ERR
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
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
static void MX_ADC1_Init(void)
{

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
	sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

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

/* TIM6 init function */
static void MX_TIM6_Init(void)
{

	TIM_MasterConfigTypeDef sMasterConfig;

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 32000;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 0;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{
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
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, PreChargeEnd_Pin | PIN_TS_ON | PIN_BMS_FAULT,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CS_6820_GPIO_Port, CS_6820_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : PreChargeEnd_Pin CS_6820_Pin PIN_TS_ON BMS_FAULT_Pin */
	GPIO_InitStruct.Pin =
	PreChargeEnd_Pin | CS_6820_Pin | PIN_TS_ON | PIN_BMS_FAULT;
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
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1)
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
