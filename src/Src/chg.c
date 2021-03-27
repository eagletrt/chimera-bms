/**
 * @file		chg.c
 * @brief		This file contains the functions to manage the chaging
 *              procedures
 *
 * @date		Oct 30, 2020
 *
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "chg.h"

#include <PID.h>
#include <can.h>

#include "pack.h"

#define PID_KP 0.5f
#define PID_KI 0.5f
#define PID_KD 0.0f

#define PID_TAU 0.02f

#define PID_OUT_MIN 0.1f
#define PID_OUT_MAX 8.0f

#define PID_LIM_MIN_INT -5.0f
#define PID_LIM_MAX_INT 5.0f

#define SAMPLE_TIME 0.02f

uint16_t do_off(fsm* FSM);
uint16_t do_start(fsm* FSM);
uint16_t do_to_tc(fsm* FSM);
uint16_t do_tc(fsm* FSM);
uint16_t do_cc(fsm* FSM);
uint16_t do_cv(fsm* FSM);

fsm fsm_chg;
PIDController pid = {PID_KP,		  PID_KI,		   PID_KD,
					 PID_TAU,		  PID_OUT_MIN,	   PID_OUT_MAX,
					 PID_LIM_MIN_INT, PID_LIM_MAX_INT, SAMPLE_TIME};

uint8_t current;

void fsm_chg_init() {
	fsm_init(&fsm_chg, CHG_NUM_STATES);
	fsm_chg.state_table[CHG_OFF] = &do_off;
	fsm_chg.state_table[CHG_START] = &do_start;
	fsm_chg.state_table[CHG_TO_TC] = &do_to_tc;
	fsm_chg.state_table[CHG_TC] = &do_tc;
	fsm_chg.state_table[CHG_CC] = &do_cc;
	fsm_chg.state_table[CHG_CV] = &do_cv;

	PIDController_Init(&pid);
}

uint16_t do_off(fsm* FSM) {
	// Nothing to do here...
	return CHG_OFF;
}

uint16_t do_start(fsm* FSM) {
	if (pack.max_voltage >= CELL_MAX_VOLTAGE) {
		return CHG_OFF;
	}

	return CHG_TO_TC;
}

uint16_t do_to_tc(fsm* FSM) {
	current = CHG_TRICKLE_CURRENT;
	can_send_chg_current(current);
	return CHG_TC;
}

uint16_t do_tc(fsm* FSM) {
	if (pack.min_voltage >= CELL_MIN_VOLTAGE) {
		return CHG_CC;
	}

	return CHG_TC;
}

uint16_t do_cc(fsm* FSM) {
	current = CHG_TRICKLE_CURRENT;

	if (pack.max_voltage >= CELL_MAX_VOLTAGE) {
		return CHG_CV;
	}
	if (current < CHG_MAX_CURRENT) {
		// Slowly increase current
		current += 1;
		can_send_chg_current(current);
	}

	return CHG_CC;
}

uint16_t do_cv(fsm* FSM) {
	// TODO: set previous measurement to keep current curve clean
	PIDController_Update(&pid, (float)CELL_MAX_VOLTAGE,
						 (float)pack.max_voltage);

	current = (uint8_t)(pid.out * 10);

	if (current < CHG_TRICKLE_CURRENT) {  // TODO: Add max charging time
		return CHG_OFF;
	}
	can_send_chg_current(current);

	return CHG_CV;
}