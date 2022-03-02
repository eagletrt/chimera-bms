/* USER CODE BEGIN Header */
/**
 * @file		main.c
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can.h"
#include "chimera_config.h"
#include "error.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMPS_READ_INTERVAL 600
#define VOLTS_READ_INTERVAL 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
state_func_t *const state_table[BMS_NUM_STATES] = {do_state_init, do_state_idle,   do_state_precharge,
												   do_state_on,	  do_state_charge, do_state_halt};

transition_func_t *const transition_table[BMS_NUM_STATES][BMS_NUM_STATES] = {
	{NULL, to_idle, to_precharge, NULL, NULL, to_halt},	 // from init
	{NULL, NULL, to_precharge, NULL, NULL, to_halt},	 // from idle
	{NULL, to_idle, NULL, to_on, to_charge, to_halt},	 // from precharge
	{NULL, to_idle, NULL, NULL, NULL, to_halt},			 // from on
	{NULL, to_idle, NULL, NULL, NULL, to_halt},			 // from charge
	{NULL, NULL, NULL, NULL, NULL, to_halt}};			 // from halt

BMS_STATE_T state = BMS_INIT;
state_global_data_t data;

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
CAN_HandleTypeDef hcan;
SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim6;

uint32_t timer_precharge = 0;
uint32_t timer_volts = 1000;
uint32_t timer_temps = 1000;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

BMS_STATE_T do_state_init(state_global_data_t *data) {
	error_init(&data->can_error);

	data->error = ERROR_OK;

	data->bms.pin_fault.gpio = BMS_FAULT_GPIO_Port;
	data->bms.pin_ts_on.gpio = TS_ON_GPIO_Port;
	data->bms.pin_chip_select.gpio = CS_6820_GPIO_Port;
	data->bms.pin_precharge_end.gpio = PreChargeEnd_GPIO_Port;

	data->bms.pin_fault.id = BMS_FAULT_Pin;
	data->bms.pin_ts_on.id = TS_ON_Pin;
	data->bms.pin_chip_select.id = CS_6820_Pin;
	data->bms.pin_precharge_end.id = PreChargeEnd_Pin;

	bms_write_pin(&(data->bms.pin_fault), GPIO_PIN_SET);
	bms_write_pin(&(data->bms.pin_ts_on), GPIO_PIN_RESET);
	bms_write_pin(&(data->bms.pin_chip_select), GPIO_PIN_SET);
	bms_write_pin(&(data->bms.pin_precharge_end), GPIO_PIN_RESET);

	pack_init(&hadc1, &(data->pack));

	HAL_Delay(1000);
	pack_update_current(&(data->pack.current), &(data->error));
	pack_zero_current(&(data->pack));

#if CHARGING > 0
	HAL_Delay(3000);
	data->bms.precharge_bypass = true;
	return BMS_PRECHARGE;
#endif
	return BMS_IDLE;
}

void to_idle(state_global_data_t *data) {
	bms_set_ts_off(&data->bms);
	can_send(&hcan, CAN_ID_BMS, CAN_MSG_TS_OFF, 8);
	HAL_CAN_ConfigFilter(&hcan, &filter_ecu);
}

BMS_STATE_T do_state_idle(state_global_data_t *data) {
	data->ts_off = false;
	if (data->ts_on) {
		return BMS_PRECHARGE;
	}
	return BMS_IDLE;
}

void to_precharge(state_global_data_t *data) {
	// Precharge

	pack_zero_current(&data->pack);

	// data->bms.precharge_bypass = true;
	bms_precharge_start(&data->bms);
	data->pack.bus_voltage = 0;
	timer_precharge = HAL_GetTick();
	HAL_CAN_ConfigFilter(&hcan, &filter_inv);
}

BMS_STATE_T do_state_precharge(state_global_data_t *data) {
	data->ts_on = false;
	if (data->ts_off) {
		data->ts_off = false;
		return BMS_IDLE;
	}
	switch (bms_precharge_check(&(data)->bms)) {
		case PRECHARGE_SUCCESS:
			// Used when bypassing precharge

			return BMS_CHARGE;
			break;

		case PRECHARGE_FAILURE:
			// Precharge timed out

			can_send_warning(&hcan, WARN_PRECHARGE_FAIL, 0);

			return BMS_IDLE;
			break;

		case PRECHARGE_WAITING:
			// If precharge is still running, send the bus voltage request

			if (data->pack.bus_voltage >= data->pack.total_voltage / 10000.0 * .70) {
				// bms_precharge_end(&data->bms);
				return BMS_ON;
			}

			if (HAL_GetTick() - timer_precharge >= 20) {
				timer_precharge = HAL_GetTick();

				// uint16_t bus_voltage = 0;
				// si8900_read_channel(&huart1, SI8900_AIN0, &bus_voltage);

				can_send(&hcan, CAN_ID_OUT_INVERTER_L, CAN_MSG_BUS_VOLTAGE, 8);
			}
			break;
	}

	return BMS_PRECHARGE;
}

void to_charge(state_global_data_t *data) {
	bms_precharge_end(&data->bms);
	can_send(&hcan, CAN_ID_BMS, CAN_MSG_TS_ON, 8);
	HAL_CAN_ConfigFilter(&hcan, &filter_ecu);
}

BMS_STATE_T do_state_charge(state_global_data_t *data) {
	data->ts_on = false;
	if (data->ts_off) {
		data->ts_off = false;
		return BMS_IDLE;
	}

	return BMS_CHARGE;
}

void to_on(state_global_data_t *data) {
	bms_precharge_end(&data->bms);
	can_send(&hcan, CAN_ID_BMS, CAN_MSG_TS_ON, 8);
	HAL_CAN_ConfigFilter(&hcan, &filter_ecu);
}

BMS_STATE_T do_state_on(state_global_data_t *data) {
	data->ts_on = false;
	if (data->ts_off) {
		data->ts_off = false;
		return BMS_IDLE;
	}

	return BMS_ON;
}

void to_halt(state_global_data_t *data) {
	bms_set_ts_off(&data->bms);
	bms_set_fault(&data->bms);

	can_send_error(&hcan, data->error, data->error_index, &data->pack);
	HAL_CAN_ConfigFilter(&hcan, &filter_ecu);
}

BMS_STATE_T do_state_halt(state_global_data_t *data) {
	data->ts_off = false;
	data->ts_on = false;
	if (data->error == ERROR_OK) {
		return BMS_IDLE;
	}
	return BMS_HALT;
}

BMS_STATE_T run_state(BMS_STATE_T state, state_global_data_t *data) {
	BMS_STATE_T new_state = state_table[state](data);

	if (data->error != ERROR_OK) {
		new_state = BMS_HALT;
	}

	transition_func_t *transition = transition_table[state][new_state];

	if (transition) {
		transition(data);
	}

	return new_state;
}

void check_timers(state_global_data_t *data) {
	uint32_t tick = HAL_GetTick();
	// Start temperature conversion ahead of time
	if (tick - timer_temps >= TEMPS_READ_INTERVAL - 4) {
		if ((VOLTS_READ_INTERVAL - 4) - (tick - timer_volts) <= 4) {
			timer_volts += 5;
		}
		_ltc6804_adcv(&hspi1, 0);
		pack_init_temperature_conversion(&hspi1);
	}
	// Read and send temperatures
	if (tick - timer_temps >= TEMPS_READ_INTERVAL) {
		timer_temps = tick;

		read_temps(data);
		ER_CHK(data->error);

		// Delay voltage measurement to avoid interferences
		timer_volts = tick - (VOLTS_READ_INTERVAL / 2);
	}

	// Start voltage conversion ahead of time
	if (tick - timer_volts >= VOLTS_READ_INTERVAL - 4) {
		if ((TEMPS_READ_INTERVAL - 4) - (tick - timer_temps) <= 4) {
			timer_temps += 5;
		}
		_ltc6804_adcv(&hspi1, 0);
	}
	// Read and send voltages and current
	if (tick - timer_volts >= VOLTS_READ_INTERVAL) {
		timer_volts = tick;

		read_volts(data);
		ER_CHK(data->error);
	}

End:;
}

/**
 * @brief Runs all the checks mandated by the rules
 * @details It runs voltage and current measurements
 */
void read_volts(state_global_data_t *data) {
	// Voltages
	WARNING_T warning = WARN_OK;

	data->error_index = pack_update_voltages(&hspi1, &data->pack, &warning, &data->error);
	can_send_pack_voltage(&hcan, data->pack);
	ER_CHK(data->error);

	if (warning != WARN_OK) {
		can_send_warning(&hcan, warning, 0);
	}

	// Current
	pack_update_current(&data->pack.current, &data->error);
	can_send_current(&hcan, data->pack.current.value, data->pack.total_voltage);
	ER_CHK(data->error);

End:;
}

void read_temps(state_global_data_t *data) {
	// Temperatures
	data->error_index = pack_update_temperatures(&hspi1, &data->pack, &data->error);
	ER_CHK(data->error);

	can_send_pack_temperature(&hcan, data->pack);

	//// Check for not healthy cells
	// uint8_t volts[PACK_MODULE_COUNT];
	// uint8_t num_cells = pack_check_voltage_drops(&data->pack, volts);

	// uint8_t i;
	// for (i = 0; i < num_cells; i++) {
	//	can_send_warning(&hcan, WARN_CELL_DROPPING, volts[i]);
	//}

End:;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	uint8_t rx_data[8] = {'\0'};
    CAN_RxHeaderTypeDef rx_header;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) {
        return;
    }

	if (rx_header.StdId == CAN_ID_IN_INVERTER_L) {
		if (rx_data[0] == CAN_IN_BUS_VOLTAGE) {
			data.pack.bus_voltage = rx_data[2] << 8;
			data.pack.bus_voltage += rx_data[1];
			data.pack.bus_voltage /= 31.99;
		}
	} else if (rx_header.StdId == CAN_ID_ECU) {
		if (rx_data[0] == CAN_IN_TS_ON) {
			data.ts_on = true;

			if (rx_data[1] == 0x01) {
				// Charge command
				data.bms.precharge_bypass = true;
			} else {
				data.bms.precharge_bypass = false;
			}
		} else if (rx_data[0] == CAN_IN_TS_OFF) {
			data.ts_off = true;
		}
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_CAN_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

	// if (HAL_CAN_Receive_IT(&hcan, CAN_FIFO0) != HAL_OK) {
	//	Error_Handler();
	//}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		state = run_state(state, &data);

		if (can_check_error(&hcan)) {
			error_set(ERROR_CAN, &data.can_error, HAL_GetTick());
		} else {
			error_unset(ERROR_CAN, &data.can_error);
		}

		data.error = error_check_fatal(&data.can_error, HAL_GetTick());
		ER_CHK(data.error);

		check_timers(&data);
		ER_CHK(data.error);

		data.error_index = pack_check_errors(&data.pack, &data.error);
		ER_CHK(data.error);

	End:;
	}
	return 0;

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
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
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_601CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */
  
  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

	can_init(&hcan);
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
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
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 32000;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PreChargeEnd_Pin|TS_ON_Pin|BMS_FAULT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_6820_GPIO_Port, CS_6820_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PreChargeEnd_Pin TS_ON_Pin BMS_FAULT_Pin */
  GPIO_InitStruct.Pin = PreChargeEnd_Pin|TS_ON_Pin|BMS_FAULT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ShutDownStatus_Pin */
  GPIO_InitStruct.Pin = ShutDownStatus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ShutDownStatus_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_6820_Pin */
  GPIO_InitStruct.Pin = CS_6820_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_6820_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	bms_set_ts_off(&data.bms);
	can_send(&hcan, CAN_ID_BMS, CAN_MSG_TS_OFF, 8);
	can_send_error(&hcan, ERROR_CAN, 0, &data.pack);

	HAL_NVIC_SystemReset();
	/* User can add his own implementation to report the HAL error
	 * return state
	 */
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and
	   line number, tex: printf("Wrong parameters value: file %s on line
	   %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

