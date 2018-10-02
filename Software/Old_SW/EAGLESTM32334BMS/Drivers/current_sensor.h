/*
 * current_sensor.h
 *
 *  Created on: 10 ago 2018
 *      Author: Utente
 */

#ifndef CURRENT_SENSOR_H_
#define CURRENT_SENSOR_H_

//	Include HAL libraries for the SPI functions
#include "stm32f3xx_hal.h"


uint32_t Get_Amps_Value(uint32_t *Vout,uint16_t offset);
float Current_Calibration_Offset(ADC_HandleTypeDef hadc1);
static int MEASUREMENT_ScaleAmps1(int rawval,uint16_t );


#endif /* CURRENT_SENSOR_H_ */

