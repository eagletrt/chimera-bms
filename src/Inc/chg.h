/**
 * @file		chg.h
 * @brief		This file contains the functions to manage the chaging
 *              procedures
 *
 * @date		Oct 30, 2020
 *
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#ifndef CHG_H
#define CHG_H

#include <fsm.h>
#include <inttypes.h>
#include <pack.h>
#include <stm32f3xx_hal.h>

typedef enum { CHG_OFF, CHG_CC, CHG_CV, CHG_NUM_STATES } chg_states;

extern fsm fsm_chg;

void fsm_chg_init(PACK_T *pack, CAN_HandleTypeDef *can);

#endif
