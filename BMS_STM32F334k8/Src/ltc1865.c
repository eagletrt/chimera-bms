/*
 * ltc1865.c
 *
 *  Created on: 30 mag 2018
 *      Author: Utente
 */


#include "ltc1865.h"




// Reads the ADC  and returns 16-bit data
void LTC1865_read(uint16_t adc_command, uint8_t *adc_code, SPI_HandleTypeDef hspi1)
{
  //spi_transfer_word(cs, (uint16_t)(adc_command<<8), adc_code);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, adc_command<<8, adc_code, 1, 100);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

}




//! Read channels in single-ended mode
//! @return void

// Calculates the LTC1867 input's bipolar voltage given the two's compliment data and lsb weight
//float LTC1867_bipolar_code_to_voltage(uint16_t adc_code, float LTC1867_lsb, int32_t LTC1867_offset_bipolar_code)
//{
//  float adc_voltage, sign = 1.0;
//  if (adc_code>>15)
//  {
//    adc_code = (adc_code ^ 0xFFFF)+1;                                           //! 1) Convert ADC code from two's complement to binary
//    sign = -1;
//  }
//  adc_voltage=((float)(adc_code+LTC1867_offset_bipolar_code))*LTC1867_lsb*sign; //! 2) Calculate voltage from ADC code, lsb, offset.
//  return(adc_voltage);
//}
void menu_1_read_single_ended(SPI_HandleTypeDef hspi1, uint8_t *tractive, uint8_t *totpack){

  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1865 command byte
  uint8_t *adc_code;                           // The LTC1865 code
  //float adc_voltage;                               // The LTC1865 voltage
   /*
      adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | uni_bi_polar;   // Build ADC command for channel 0
      LTC1865_read(adc_command, adc_code,hspi1);             // Throws out last reading
     */ HAL_delay(100);
      uint8_t x;                                                    //!< iteration variable
    for (x = 0; x <= 1; x++)                                      // Read all channels in single-ended mode
      {
        adc_command = BUILD_COMMAND_SINGLE_ENDED[x] | uni_bi_polar;
        LTC1865_read(adc_command, adc_code[x], hspi1);

     }
    *tractive = adc_code[0];
    *totpack = adc_code[1];

}

//! Read channels in differential mode
//! @return void
void menu_2_read_differential(SPI_HandleTypeDef hspi1)
{
  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1865 command byte
  uint16_t adc_code = 0;                           // The LTC1865 code
  float adc_voltage;                               // The LTC1865 voltage
  while (1)
  {
//    if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
//      Serial.println(F("Differential, Unipolar mode:"));
//    else
//      Serial.println(F("Differential, Bipolar mode:"));
//
//    Serial.println(F("*************************"));
//    Serial.println(F("0-0P-1N"));
//    Serial.println(F("1-2P-3N"));
//    Serial.println(F("2-4P-5N"));
//    Serial.println(F("3-6P-7N"));
//    Serial.println(F("4-1P-0N"));
//    Serial.println(F("5-3P-2N"));
//    Serial.println(F("6-5P_4N"));
//    Serial.println(F("7-7P-6N"));
//    Serial.println(F("8-ALL Even_P-Odd_N"));
//    Serial.println(F("9-ALL Odd_P-Even_N"));
//    Serial.println(F("m-Main Menu"));
//    Serial.println(F("Enter a Command: "));

//    user_command = read_int();  // Read the channel for differential bipolar mode
//
//    if (user_command == 'm')
//      return;
////    Serial.println(user_command);
//    int8_t y;                   //!< Variable to add to argument to BUILD_COMMAND_DIFF to select polarity.
//
//    if ((user_command == 8) | (user_command == 9))
//    {
//      if (user_command == 8)    // Cycles through options 0-3
//      {
////        Serial.println(F("ALL Even_P-Odd_N"));
//        y = 0;
//      }
//      else
//      {
////        Serial.println(F("ALL Odd_P-Even_N"));
//        y = 4;
//      }

      adc_command = BUILD_COMMAND_DIFF[0] | uni_bi_polar;   // Build ADC command for first channel in bipolar mode
      LTC1865_read(adc_command, &adc_code,hspi1);     // Throws out last reading
      delay(100);
      uint8_t x;                                            //!< iteration variable
      for (x = 0; x <= 1; x++)                              // Read all channels in bipolar mode. All even channels are positive and odd channels are negative
      {
        adc_command = BUILD_COMMAND_DIFF[x] | uni_bi_polar;
        LTC1865_read(adc_command, &adc_code,hspi1);
   //     if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)

          adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);


//        else
//          adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
//        Serial.println();
//        Serial.print(F("  ****"));
       // print_user_command(x + y);
//        Serial.print(F(": "));
//        Serial.print(adc_voltage, 4);
//        Serial.println(F("V"));
      }
    }
//    else
//    {
//      // Read and display a selected channel
//      adc_command = BUILD_COMMAND_DIFF[user_command] | uni_bi_polar;
//      Serial.print(F("ADC Command: B"));
//      Serial.println(adc_command, BIN);
//      LTC1867_read(LTC1867_CS, adc_command, &adc_code); // Throws out last reading
//      delay(100);
//      LTC1867_read(LTC1867_CS, adc_command, &adc_code);
//      Serial.print(F("Received Code: 0x"));
//      Serial.println(adc_code, HEX);
//      if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
//        adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);
//      else
//        adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
//      Serial.println();
//      Serial.print(F("  ****"));
//      print_user_command(user_command);
//      Serial.print(adc_voltage, 4);
//      Serial.println(F("V"));
//    }
//  }
}
// Calculates the LTC1867 input's unipolar voltage given the binary data and lsb weight.
float LTC1867_unipolar_code_to_voltage(uint16_t adc_code, float LTC1867_lsb, int32_t LTC1867_offset_unipolar_code)
{
  float adc_voltage;
  adc_voltage=((float)(adc_code+LTC1867_offset_unipolar_code))*LTC1867_lsb;   //! 1) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}
