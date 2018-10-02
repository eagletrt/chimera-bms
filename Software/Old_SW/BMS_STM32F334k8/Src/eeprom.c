/*
 * eeprom.c



 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */
#include "eeprom.h"
/*----- i2c Write Function -----*/
void i_write(unsigned position, unsigned int data,I2C_HandleTypeDef hi2c1){

	int count=0;
	if(data>255){
		while(data>255){
			data=data-255;
			count++;
		}
	}

	if(count>0){
		//for writing to EEPROM
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
		HAL_Delay(5);
	}else{
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
	    HAL_Delay(5);
	}
}

/*----- i2c Write Function -----*/
unsigned int i_read(unsigned position, I2C_HandleTypeDef hi2c1){

	int count=0;
	unsigned int data=0;
	//for reading to EEPROM
	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position, 0xFF, (uint8_t*)&count,1,1);
	HAL_I2C_Mem_Read(&hi2c1,M24M02DRC_1_DATA_ADDRESS, position+1, 0xFF, (uint8_t*)&data,1,1);
	data=data+(count*255);
	return data;

}
/*----- Scan i2c Addresses -----*/
uint8_t retrieveI2Cid( I2C_HandleTypeDef hi2c1){

	uint8_t i;
	for(i=0; i<255; i++){

			if(HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 10) == HAL_OK){
		    //HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12);
			break;
			}
			return i;
	}

}

