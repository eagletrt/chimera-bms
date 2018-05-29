/*
 * adc_hv.h
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */

#ifndef LTC185_H_
#define LTC185_H_
#include "stm32f3xx_hal.h"

// Single-Ended Channel Address When CH7/COM Pin Is used As CH7
#define LTC1867_CH0             0x80      // 0000000000000000 1000000000000000
#define LTC1867_CH1             0xC0  //  0000000000000000 1100000000000000

// Differential Channel Address When CH7/COM Pin Is Used As CH7
#define LTC1867_P0_N1           0x00  //0000000000000000 00 00000000000000
#define LTC1867_P1_N0           0x40 //0000000000000000 01 00000000000000





/*
  Example command
  adc_command = LTC1867_P0_N1;                             // Differential Bipolar Mode with CH0 as positive and CH1 as negative.
  adc_command = LTC1867_P0_N1 | LTC1867_UNIPOLAR_MODE;     // Differential Unipolar Mode with CH0 as positive and CH1 as negative.
 */

//! Reads the ADC and returns 16-bit data
//! @return void
void LTC1865_read(uint8_t cs,           //!< Chip Select Pin
                  uint16_t adc_command,  //!< Channel address, config bits ORed together
                  uint16_t *adc_code ,   //!< Returns code read from ADC (from previous conversion)
				  SPI_HandleTypeDef hspi1
);

#endif /* LTC187_H_ */
