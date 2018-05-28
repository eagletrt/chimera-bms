/*
 * eeprom.h
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#include "stm32f3xx_hal.h"


#define M24M02DRC_1_DATA_ADDRESS 0x50 // Address of the first 1024 page M24M02DRC EEPROM data buffer, 2048 bits (256 8-bit bytes) per page
#define M24M02DRC_1_IDPAGE_ADDRESS 0x58 // Address of the single M24M02DRC lockable ID page of the first EEPROM
#define M24M02DRC_2_DATA_ADDRESS 0x54 // Address of the second 1024 page M24M02DRC EEPROM data buffer, 2048 bits (256 8-bit bytes) per page
#define M24M02DRC_2_IDPAGE_ADDRESS 0x5C // Address of the single M24M02DRC lockable ID page of the second EEPROM

void i_write(unsigned position, unsigned int data,I2C_HandleTypeDef hi2c1);
unsigned int i_read(unsigned position,I2C_HandleTypeDef hi2c1);
uint8_t retrieveI2Cid( I2C_HandleTypeDef hi2c1);


#endif /* EEPROM_H_ */
