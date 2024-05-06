/*
*   DAC8532 STM32F7 SPI Driver
*
*   Author:     Ethan Garnier
*   Date:       May 2nd, 2024 
*/
#ifndef DAC8532_SPI_DRIVER_H
#define DAC8532_SPI_DRIVER_H

#include "stm32f7xx_hal.h" /* Needed for SPI */
#include "stdint.h"

/* Chip Select on pin PF12 by default */
#define CS_GPIO_PORT GPIOF
#define CS_GPIO_PIN GPIO_PIN_12

/* Control bytes to be sent as the first byte to DAC8532 */
#define LOAD_CHANNEL_A 0x10 
#define LOAD_CHANNEL_B 0x24
#define WRITE_CHANNEL_A 0x00
#define WRITE_CHANNEL_B 0x04
#define DAC_MAX_VALUE 65535
#define DAC_MIN_VALUE 0
#define DAC_VREF 3.3

/* Sensor Struct */
typedef struct
{
    // STM32F7 HAL specific handler for SPI communication
    SPI_HandleTypeDef *spiHandle;

    // 24 bit buffer that is sent to DAC8532 on each write
    //  - buffer[0:7]:      command byte that tells the DAC to WRITE or LOAD data and to which channel (A/B)
    //  - buffer[8:23]:     16 bit value to be converted into an analog signal by the DAC
    uint8_t buffer[3];
} DAC8532;

HAL_StatusTypeDef DAC8532_Init(DAC8532 *dac, SPI_HandleTypeDef *spiHandle);
HAL_StatusTypeDef DAC8532_Write_Data(DAC8532 *dac, uint8_t command, uint16_t data);
HAL_StatusTypeDef DAC8532_Output_Voltage(DAC8532 *dac, uint8_t command, float voltage);

#endif