/**
 * @file		chg.h
 * @brief		This file contains the functions to manage the chaging
 *              procedures
 *
 * @date		Oct 30, 2020
 *
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#pragma once

#include <fsm.h>
#include <inttypes.h>

#include "can.h"
#include "can_comm.h"
#include "pack.h"

typedef enum {
	CHG_OFF,
	CHG_START,
	CHG_TO_TC,
	CHG_TC,
	CHG_CC,
	CHG_CV,
	CHG_NUM_STATES
} chg_states;

extern fsm fsm_chg;

void fsm_chg_init();
