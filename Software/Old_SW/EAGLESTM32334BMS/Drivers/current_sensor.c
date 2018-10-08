/*
 * current_sensor.c

 *
 *  Created on: 10 ago 2018
 *      Author: Utente
 */
#include "current_sensor.h"

float Current_Calibration_Offset(ADC_HandleTypeDef hadc1){
	uint32_t *adcBuffer;
	uint32_t sum=0;
	for(int i=0;i<10;i++){


	HAL_ADC_Start_DMA(&hadc1, adcBuffer[i], 1);

	sum=sum+adcBuffer[i];
	HAL_Delay(20);

	}
	float offset=(float)sum/10;
	return offset;
}
static int MEASUREMENT_ScaleAmps1(int rawval,uint16_t sensor_offset){


	return (((rawval-sensor_offset)*99)/512);
}
uint32_t Get_Amps_Value(uint32_t *Vout,uint16_t offset){




		float VVoutV = (float)Vout[0] * 3.3 / 4095;
		return MEASUREMENT_ScaleAmps1((int)Vout[0], offset);

	}

