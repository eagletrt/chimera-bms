/**
 ******************************************************************************
 * @file	ltc_68xx.h
 * @author	Dona, Riki, Gregor e Davide
 * @brief	This file contains all the functions properties for the LTC68xx
 * 			battery management
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ltc_68xx_H
#define ltc_68xx_H

//	Include HAL libraries for the SPI functions
#include "stm32f3xx_hal.h"

/** @defgroup PackState Battery Pack Status
 */
typedef enum
{
	PACK_OK					= 0x00U,  /*!< Pack is OK */
	UNDER_VOLTAGE			= 0x01U,  /*!< Cell in Under Voltage */
	OVER_VOLTAGE			= 0x02U,  /*!< Cell in Over Voltage */
	OVER_TEMPERATURE		= 0x03U,  /*!< Cell in Over Temperature */
	PACK_OVER_TEMPERATURE	= 0x04U,  /*!< Pack in Over Temperature */
	DATA_NOT_UPDATED		= 0x05U,  /*!< Data not received form LTC68xx for more that 1000 cycles */
}PackStateTypeDef;

uint16_t pec15(uint8_t len,uint8_t data[], uint16_t crcTable[]);
uint16_t convert_voltage(uint8_t v_data[]);
uint16_t convert_temp(uint16_t volt);
void wakeup_idle(SPI_HandleTypeDef *hspi);
PackStateTypeDef status(uint16_t cell_voltages[108][2],
			   uint16_t cell_temperatures[108][2],
			   uint32_t* pack_v,
			   uint16_t* min_v,
			   uint16_t* max_v,
			   uint16_t* avg_v,
			   uint16_t* max_t,
			   uint16_t* avg_t);
void ltc6804_rdcv_temp(uint8_t ic_n, uint8_t parity, uint16_t cell_temp[108][2],	SPI_HandleTypeDef *hspi);
void ltc6804_rdcv_voltages(uint8_t ic_n, uint16_t cell_voltages[108][2],	SPI_HandleTypeDef *hspi);
void ltc6804_command_temperatures(uint8_t start, uint8_t parity, SPI_HandleTypeDef *hspi);
void ltc6804_adcv(uint8_t DCP, SPI_HandleTypeDef *hspi);

#endif /*ltc_68xx_H*/
