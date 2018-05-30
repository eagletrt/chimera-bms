#ifndef LTC185_H_
#define LTC185_H_

#include "stm32f3xx_hal.h"

// Single-Ended Channel Address When CH7/COM Pin Is used As CH7
#define LTC1867_CH0             0x80      // 0000000000000000 1000000000000000
#define LTC1867_CH1             0xC0  //  0000000000000000 1100000000000000

// Differential Channel Address When CH7/COM Pin Is Used As CH7
#define LTC1867_P0_N1           0x00  //0000000000000000 00 00000000000000
#define LTC1867_P1_N0           0x40 //0000000000000000 01 00000000000000



//! @name LTC1867 Sleep / Unipolar/Bipolar config bits

// Sleep Mode Command
#define LTC1867_SLEEP_MODE      0x02
#define LTC1867_EXIT_SLEEP_MODE 0x00
#define LTC1867_UNIPOLAR_MODE   0x04  // Bitwise OR with channel commands for unipolar mode
//#define LTC1867_BIPOLAR_MODE    0x00

// Global variables
static uint8_t demo_board_connected;                    //!< Set to 1 if the board is connected
static uint8_t uni_bi_polar = LTC1867_UNIPOLAR_MODE;    //!< The LTC1867 unipolar/bipolar mode selection
static float LTC1867_lsb = 6.25009537E-5;               //!< Ideal LSB voltage for a perfect part
static int32_t LTC1867_offset_unipolar_code = 0;        //!< Ideal unipolar offset for a perfect part
//static int32_t LTC1867_offset_bipolar_code = 0;         //!< Ideal bipolar offset for a perfect part

//! Lookup table to build the command for single-ended mode, input with respect to GND
 static const  uint8_t BUILD_COMMAND_SINGLE_ENDED[1] = {LTC1867_CH0, LTC1867_CH1
                                              }; //!< Builds the command for single-ended mode, input with respect to GND
 //! Lookup table to build the command for differential mode with the selected uni/bipolar mode
 static const uint8_t BUILD_COMMAND_DIFF[1] = {LTC1867_P0_N1,
                                        LTC1867_P1_N0
                                       }; //!< Build the command for differential mode

 /*
   Example command
   adc_command = LTC1867_P0_N1;                             // Differential Bipolar Mode with CH0 as positive and CH1 as negative.
   adc_command = LTC1867_P0_N1 | LTC1867_UNIPOLAR_MODE;     // Differential Unipolar Mode with CH0 as positive and CH1 as negative.
  */

 //! Reads the ADC and returns 16-bit data
 //! @return void
 void LTC1865_read(//uint8_t cs,           //!< Chip Select Pin
                   uint16_t adc_command,  //!< Channel address, config bits ORed together
                   uint16_t *adc_code ,   //!< Returns code read from ADC (from previous conversion)
 				  SPI_HandleTypeDef hspi1
 );
 float LTC1867_unipolar_code_to_voltage(uint16_t adc_code,
 										float LTC1867_lsb,
 										int32_t LTC1867_offset_unipolar_code);
 void menu_1_read_single_ended(SPI_HandleTypeDef hspi1);
 void menu_2_read_differential(SPI_HandleTypeDef hspi1);


#endif /* LTC185_H_ */
