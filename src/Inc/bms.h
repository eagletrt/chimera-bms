/**
 * @file		bms.h
 * @brief		This file contains the functions to manage the battery
 * pack procedures
 *
 * @date		May 21, 2019
 * @author	Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef BMS_H_
#define BMS_H_

#include "stm32f3xx_hal.h"
#include <stdbool.h>

typedef enum {
  PRECHARGE_SUCCESS,
  PRECHARGE_FAILURE,
  PRECHARGE_WAITING
} PRECHARGE_STATE;

typedef struct {
  GPIO_TypeDef *gpio;
  uint16_t id;
  GPIO_PinState state;

} BMS_PIN_T;

typedef struct {
  BMS_PIN_T pin_fault;
  BMS_PIN_T pin_ts_on;
  BMS_PIN_T pin_chip_select;
  BMS_PIN_T pin_precharge_end;

  uint32_t precharge_timestamp;
  bool precharge_bypass;

} BMS_CONFIG_T;

extern BMS_CONFIG_T bms;

void bms_write_pin(BMS_PIN_T *pin, GPIO_PinState state);
void bms_precharge_start();
PRECHARGE_STATE bms_precharge_check();
void bms_precharge_end();
void bms_set_ts_off();
void bms_set_fault();

#endif /* BMS_H_ */