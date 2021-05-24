/**
 * @file		fsm_bms.c
 * @brief		Finite state machine for the BMS
 *
 * @date		Dec 05, 2020
 * @author      Matteo Bonora [matteo.bonora@studenti.unitn.it]
 */

#include "fsm_bms.h"

#include "adc.h"
#include "bms.h"
#include "can.h"
#include "can_comm.h"
#include "chg.h"
#include "dma.h"

bms_states do_init(fsm *FSM);
bms_states to_idle(fsm *FSM);
bms_states do_idle(fsm *FSM);
bms_states to_precharge(fsm *FSM);
bms_states do_precharge(fsm *FSM);
bms_states to_on(fsm *FSM);
bms_states do_on(fsm *FSM);
bms_states to_halt(fsm *FSM);
bms_states do_halt(fsm *FSM);

fsm fsm_bms;
uint32_t timer_precharge = 0;
ERROR_STATUS_T can_error;

CAN_RxHeaderTypeDef can_rx;
uint8_t data[8];

void fsm_bms_init() {
	fsm_init(&fsm_bms, BMS_NUM_STATES);
	fsm_bms.state_table[BMS_INIT] = &do_init;
	fsm_bms.state_table[BMS_IDLE] = &do_idle;
	fsm_bms.state_table[BMS_PRECHARGE] = &do_precharge;
	fsm_bms.state_table[BMS_ON] = &do_on;
	fsm_bms.state_table[BMS_HALT] = &do_halt;
}

bms_states do_init(fsm *FSM) {
	error_init(&can_error);

	error = ERROR_OK;

	bms.pin_fault.gpio = BMS_FAULT_GPIO_Port;
	bms.pin_ts_on.gpio = TS_ON_GPIO_Port;
	bms.pin_chip_select.gpio = CS_6820_GPIO_Port;
	bms.pin_precharge_end.gpio = PreChargeEnd_GPIO_Port;

	bms.pin_fault.id = BMS_FAULT_Pin;
	bms.pin_ts_on.id = TS_ON_Pin;
	bms.pin_chip_select.id = CS_6820_Pin;
	bms.pin_precharge_end.id = PreChargeEnd_Pin;

	bms_write_pin(&(bms.pin_fault), GPIO_PIN_SET);
	bms_write_pin(&(bms.pin_ts_on), GPIO_PIN_RESET);
	bms_write_pin(&(bms.pin_chip_select), GPIO_PIN_SET);
	bms_write_pin(&(bms.pin_precharge_end), GPIO_PIN_RESET);

	can_init();
	pack_init(&hadc1, &pack);
	fsm_chg_init(&pack, &hcan);

#if CHARGING > 0
	HAL_Delay(10000);
	data->bms.precharge_bypass = true;
	return BMS_PRECHARGE;
#endif
	return BMS_IDLE;
}

bms_states to_idle(fsm *FSM) {
	bms_set_ts_off();
	HAL_CAN_ConfigFilter(&hcan, &CAN_FILTER_NORMAL);
	can_send(CAN_ID_BMS, CAN_MSG_TS_OFF, 8);

	return BMS_IDLE;
}

bms_states do_idle(fsm *FSM) {
	can_receive(&can_rx, data);

	if (can_rx.StdId == CAN_ID_ECU) {
		if (data[0] == CAN_IN_TS_ON) {
			// TS On
			if (data[1] == 0x01) {
				// Charge command
				bms.precharge_bypass = true;
			}
			return BMS_PRECHARGE;
		}
	}
	return BMS_IDLE;
}

bms_states to_precharge(fsm *FSM) {
	// Precharge
	bms_precharge_start();
	timer_precharge = HAL_GetTick();

	HAL_CAN_ConfigFilter(&hcan, &CAN_FILTER_PRECHARGE);
	can_send(CAN_ID_OUT_INVERTER_L, CAN_MSG_BUS_VOLTAGE, 8);

	return BMS_PRECHARGE;
}

bms_states do_precharge(fsm *FSM) {
	// Check for incoming voltage
	can_receive(&can_rx, data);

	if (can_rx.StdId == CAN_ID_IN_INVERTER_L) {
		if (data[0] == CAN_IN_BUS_VOLTAGE) {
			uint16_t bus_voltage = 0;

			bus_voltage = data[2] << 8;
			bus_voltage += data[1];
			bus_voltage /= 31.99;

			if (bus_voltage >= pack.total_voltage / 10000 * 0.95) {
				bms_precharge_end();
				return BMS_ON;
			}
		}
	}

	switch (bms_precharge_check()) {
		case PRECHARGE_SUCCESS:
			// Used when bypassing precharge

			bms_precharge_end();
			return BMS_ON;
			break;

		case PRECHARGE_FAILURE:
			// Precharge timed out

			can_send_warning(WARN_PRECHARGE_FAIL, 0);

			return BMS_IDLE;
			break;

		case PRECHARGE_WAITING:
			// If precharge is still running, send the bus voltage request

			if (HAL_GetTick() - timer_precharge >= 20) {
				timer_precharge = HAL_GetTick();

				can_send(CAN_ID_OUT_INVERTER_L, CAN_MSG_BUS_VOLTAGE, 8);
			}
			break;
	}
	return BMS_PRECHARGE;
}

bms_states to_on(fsm *FSM) {
	bms_precharge_end();
	HAL_CAN_ConfigFilter(&hcan, &CAN_FILTER_NORMAL);
	can_send(CAN_ID_BMS, CAN_MSG_TS_ON, 8);

	return BMS_ON;
}

bms_states do_on(fsm *FSM) {
	can_receive(&can_rx, data);

	if (can_rx.StdId == CAN_ID_ECU) {
		if (data[0] == CAN_IN_TS_OFF) {
			return BMS_IDLE;
		}
	} else if (can_rx.StdId == CAN_ID_HANDCART) {
		if (data[0] == CAN_IN_CHG_START) {
			// if (fsm_get_state(&fsm_chg) == CHG_OFF) {
			//	// trigger charge start
			//	fsm_set_state(&fsm_chg, CHG_CC);
			//}
		}
	}

	// Run charging state machine
	// fsm_run(&fsm_chg);

	return BMS_ON;
}

bms_states to_halt(fsm *FSM) {
	bms_set_ts_off();
	bms_set_fault();

	can_send_error(error, error_index, &pack);

	return BMS_HALT;
}

bms_states do_halt(fsm *FSM) { return BMS_HALT; }
