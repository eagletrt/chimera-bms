/* USER CODE BEGIN Header */
/**
 * @file		main.c
 *
 * @author	Gregor
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bms.h"
#include "error.h"
#include "pack.h"
/* USER CODE END Includes */

/* Exported types
 * ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
	BMS_CONFIG_T bms;
	PACK_T pack;

	CanRxMsgTypeDef can_rx;
	ERROR_STATUS_T can_error;

	ERROR_T error;
	uint8_t error_index;

} state_global_data_t;

typedef enum {
	BMS_INIT,
	BMS_IDLE,
	BMS_PRECHARGE,
	BMS_ON,
	BMS_CHARGE,
	BMS_HALT,
	BMS_NUM_STATES
} BMS_STATE_T;

typedef BMS_STATE_T state_func_t(state_global_data_t *data);
typedef void transition_func_t(state_global_data_t *data);
/* USER CODE END ET */

/* Exported constants
 * --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro
 * ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes
 * ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
BMS_STATE_T do_state_init(state_global_data_t *data);
BMS_STATE_T do_state_idle(state_global_data_t *data);
BMS_STATE_T do_state_precharge(state_global_data_t *data);
BMS_STATE_T do_state_on(state_global_data_t *data);
BMS_STATE_T do_state_charge(state_global_data_t *data);
BMS_STATE_T do_state_halt(state_global_data_t *data);

void to_idle(state_global_data_t *data);
void to_precharge(state_global_data_t *data);
void to_on(state_global_data_t *data);
void to_charge(state_global_data_t *data);
void to_halt(state_global_data_t *data);

void check_timers(state_global_data_t *data);
void read_volts(state_global_data_t *data);
void read_temps(state_global_data_t *data);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CurrentSensor_Pin GPIO_PIN_0
#define CurrentSensor_GPIO_Port GPIOA
#define PreChargeEnd_Pin GPIO_PIN_1
#define PreChargeEnd_GPIO_Port GPIOA
#define CS_6820_Pin GPIO_PIN_3
#define CS_6820_GPIO_Port GPIOA
#define ShutDownStatus_Pin GPIO_PIN_6
#define ShutDownStatus_GPIO_Port GPIOA
#define TS_ON_Pin GPIO_PIN_9
#define TS_ON_GPIO_Port GPIOA
#define BMS_FAULT_Pin GPIO_PIN_10
#define BMS_FAULT_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
