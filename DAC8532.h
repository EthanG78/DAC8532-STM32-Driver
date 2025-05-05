/*
 *   DAC8532 STM32F7 SPI Driver
 *
 *   Author:     Ethan Garnier
 *   Date:       May 2nd, 2024
 */
#ifndef DAC8532_SPI_DRIVER_H
#define DAC8532_SPI_DRIVER_H

#include "stm32f7xx_hal.h" /* Needed for SPI */

/* Control bytes to be sent as the first byte to DAC8532 */
#define LOAD_CHANNEL_A 0x10
#define LOAD_CHANNEL_B 0x24
#define WRITE_CHANNEL_A 0x00
#define WRITE_CHANNEL_B 0x04
#define DAC_MAX_VALUE 65535
#define DAC_MIN_VALUE 0
#define DAC_VREF 3.3f

/* Sensor Struct */
typedef struct
{
    // STM32F7 HAL specific handler for SPI communication
    SPI_HandleTypeDef *spiHandle;

    // Port where the chip select GPIO pin is located
    GPIO_TypeDef *csPort;

    // 16 bit pin number of the chip select GPIO pin
    unsigned short csPin;
} DAC8532;

HAL_StatusTypeDef DAC8532_Init (DAC8532 *dac,
                                SPI_HandleTypeDef *spiHandle,
                                GPIO_TypeDef *csPort,
                                unsigned short csPin);
HAL_StatusTypeDef DAC8532_Write_Data (DAC8532 *dac, unsigned char command, unsigned short data);
HAL_StatusTypeDef DAC8532_Output_Voltage (DAC8532 *dac, unsigned char command, float voltage);

#endif
