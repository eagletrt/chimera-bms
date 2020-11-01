/**
 * @file		chg.c
 * @brief		This file contains the functions to manage the chaging
 *              procedures
 *
 * @date		Oct 30, 2020
 *
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include <PID.h>
#include <can.h>
#include <chg.h>

#define PID_KP 0.5f
#define PID_KI 0.5f
#define PID_KD 0.0f

#define PID_TAU 0.02f

#define PID_OUT_MIN 0.1f
#define PID_OUT_MAX 8.0f

#define PID_LIM_MIN_INT -5.0f
#define PID_LIM_MAX_INT 5.0f

#define SAMPLE_TIME 0.01f

uint16_t do_off(fsm* FSM);
uint16_t do_cc(fsm* FSM);
uint16_t do_cv(fsm* FSM);

fsm fsm_chg;
PACK_T* pack;
CAN_HandleTypeDef* can;
PIDController pid = {PID_KP,		  PID_KI,		   PID_KD,
					 PID_TAU,		  PID_OUT_MIN,	   PID_OUT_MAX,
					 PID_LIM_MIN_INT, PID_LIM_MAX_INT, SAMPLE_TIME};

void fsm_chg_init(PACK_T* pack, CAN_HandleTypeDef* can) {
	fsm_init(&fsm_chg, CHG_NUM_STATES);
	fsm_chg.state_table[CHG_OFF] = &do_off;
	fsm_chg.state_table[CHG_CC] = &do_cc;
	fsm_chg.state_table[CHG_CV] = &do_cv;

	PIDController_Init(&pid);

	pack = pack;
	can = can;
}

uint16_t do_off(fsm* FSM) {
	// Nothing to do here...
	return CHG_OFF;
}

uint16_t do_cc(fsm* FSM) {
	// request maximum current and hope for the best
	can_send_chg_current(can, CHG_MAX_CURRENT);

	if (pack->max_voltage >= CELL_MAX_VOLTAGE) {
		return CHG_CV;
	}

	return CHG_CC;
}

uint16_t do_cv(fsm* FSM) {
	PIDController_Update(&pid, CELL_MAX_VOLTAGE, pack->current.value / 10.0f);

	if (pid.out < 0.1) {  // TODO: Add max charging time
		return CHG_OFF;
	}
	can_send_chg_current(can, pid.out);

	return CHG_CV;
}