
#include <LTC1865.h>


// Reads the ADC  and returns 16-bit data
void LTC1865_read(uint8_t cs, uint16_t adc_command, uint16_t *adc_code, SPI_HandleTypeDef hspi1)
{
  //spi_transfer_word(cs, (uint16_t)(adc_command<<8), adc_code);

  HAL_SPI_TransmitReceive(&hspi1, adc_command<<16, adc_code, 1, 100);

  //spi Transfer receive
}
