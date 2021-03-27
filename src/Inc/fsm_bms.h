/**
 * @file		fsm_bms.h
 * @brief		Finite state machine for the BMS
 *
 * @date		Dec 05, 2020
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */
#pragma once

#include <inttypes.h>

#include "error.h"
#include "fsm.h"

typedef enum {
  BMS_INIT,
  BMS_IDLE,
  BMS_PRECHARGE,
  BMS_ON,
  BMS_HALT,
  BMS_NUM_STATES
} bms_states;

extern fsm fsm_bms;

void fsm_bms_init();
