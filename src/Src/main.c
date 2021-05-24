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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_comm.h"
#include "chg.h"
#include "chimera_config.h"
#include "error.h"
#include "fsm_bms.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMPS_READ_INTERVAL 200
#define VOLTS_READ_INTERVAL 40
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ERROR_T error;
uint8_t error_index;

uint32_t timer_volts = 0;
uint32_t timer_temps = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void check_timers() {
	uint32_t tick = HAL_GetTick();
	// Read and send temperatures
	if (tick - timer_temps >= TEMPS_READ_INTERVAL) {
		timer_temps = tick;

		read_temps();
		ER_CHK(&error);

		// Delay voltage measurement to avoid interferences
		timer_volts = HAL_GetTick() - (VOLTS_READ_INTERVAL / 2);
	}

	// Read and send voltages and current
	if (tick - timer_volts >= VOLTS_READ_INTERVAL) {
		timer_volts = tick;

		read_volts();
		if (CHARGING) {
			// TODO: safety checks
			fsm_run(&fsm_chg);
			uint8_t state = fsm_get_state(&fsm_chg);
			can_send_chg_state(state);
		}
		ER_CHK(&error);
	}

End:;
}

/**
 * @brief Runs all the checks mandated by the rules
 * @details It runs voltage and current measurements
 */
void read_volts() {
	// Voltages
	WARNING_T warning = WARN_OK;

	// error_index = pack_update_voltages(&hspi1, &pack, &warning, &error);
	// ER_CHK(&error);

	if (warning != WARN_OK) {
		can_send_warning(warning, 0);
	}

	// Current
	pack_update_current(&pack.current, &error);
	ER_CHK(&error);

	// Update total values
	can_send_current(pack.current.value, pack.total_voltage);
	can_send_pack_voltage(pack);

End:;
}

void read_temps() {
	// Temperatures
	error_index = pack_update_temperatures(&hspi1, &pack, &error);
	ER_CHK(&error);

	can_send_pack_temperature(pack);

	// Check for not healthy cells
	uint8_t volts[PACK_MODULE_COUNT];
	uint8_t num_cells = pack_check_voltage_drops(&pack, volts);

	uint8_t i;
	for (i = 0; i < num_cells; i++) {
		can_send_warning(WARN_CELL_DROPPING, volts[i]);
	}

End:;
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
	// fsm_bms_init(&fsm_bms);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// fsm_run(&fsm_bms);

		// if (can_check_error(&hcan)) {
		//  error_set(ERROR_CAN, &can_error, HAL_GetTick());
		//} else {
		//  error_unset(ERROR_CAN, &can_error);
		//}

		// error = error_check_fatal(&can_error, HAL_GetTick());
		// ER_CHK(&error);

		check_timers();
		ER_CHK(&error);

		error_index = pack_check_errors(&pack, &error);
		ER_CHK(&error);

	End:;

		// switch (can_rx.StdId) {}

		// Check precharge timeout
		/*if (bms.status == BMS_PRECHARGE) {
				switch (bms_precharge_check(&bms)) {
						case BMS_ON:
								// Used when bypassing precharge
								can_send(&hcan, CAN_ID_BMS, CAN_MSG_TS_ON,
		8); HAL_CAN_ConfigFilter(&hcan, &CAN_FILTER_NORMAL);

								break;
						case BMS_OFF:
								// Precharge timed out
								HAL_CAN_ConfigFilter(&hcan,
		&CAN_FILTER_NORMAL); can_send_warning(&hcan, WARN_PRECHARGE_FAIL,
		0); break; case BMS_PRECHARGE:
								// If precharge is still running, send the
		bus voltage
								// request
								if (HAL_GetTick() - timer_precharge >= 20) {
										timer_precharge = HAL_GetTick();
										can_send(&hcan,
		CAN_ID_OUT_INVERTER_L, CAN_MSG_INVERTER_VOLTAGE, 8);
								}
								break;
						default:
								break;
				}
		}*/
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

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
	/* User can add his own implementation to report the file name and line
	   number, tex: printf("Wrong parameters value: file %s on line %d\r\n",
	   file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
