/*
*   DAC8532 STM32F7 SPI Driver
*   
*   Author:     Ethan Garnier
*   Date:       May 2nd, 2024
*/
#include "DAC8532.h"

/**
 * DAC8532_Init(DAC8532 *dac, SPI_HandleTypeDef *spiHandle)
 * 
 * Initialize the passed DAC8532 struct with the provided STM32 SPI handler 
 * and send some initial dummy data (0x0000) to ensure SPI connection is working
 * 
 * Returns a HAL_StatusTypeDef indiciating the success of the operation.
*/
HAL_StatusTypeDef DAC8532_Init(DAC8532 *dac, SPI_HandleTypeDef *spiHandle)
{
    HAL_StatusTypeDef status;

    dac->spiHandle = spiHandle;
    dac->buffer[0] = 0x00;
    dac->buffer[1] = 0x00;
    dac->buffer[2] = 0x00;

    do
    {
        status = DAC8532_Write_Data(dac, WRITE_CHANNEL_A, 0x0000); 
        HAL_Delay(10);
    } while (status == HAL_BUSY);

    return status;
}

/**
 * DAC8532_Write_Data(DAC8532 *dac, uint8_t command, uint16_t data)
 * 
 * Send 16 bits of data to the DAC8532 initialized by *dac using the 
 * the provided 8 bit command. There are 4 acceptable 8 bit commands:
 *      - LOAD_CHANNEL_A    (0x10): Write to data buffer A and load DAC A
 *      - LOAD_CHANNEL_B    (0x24): Write to data buffer B and load DAC B
 *      - WRITE_CHANNEL_A   (0x00): Write to data buffer A (no output on VoutA)
 *      - WRITE_CHANNEL_B   (0x04): Write to data buffer B (no output on VoutB)
 * 
 * The above commands may be OR'd together, for example:
 *      (LOAD_CHANNEL_A | LOAD_CHANNEL_B)
 *      Write to data buffer B and load DAC A and DAC B simultaneously.
 * 
 * Returns a HAL_StatusTypeDef indicating the success of the operation.
*/
HAL_StatusTypeDef DAC8532_Write_Data(DAC8532 *dac, uint8_t command, uint16_t data)
{
    dac->buffer[0] = command;
    dac->buffer[1] = data >> 8;
    dac->buffer[2] = data & 0xFF;
    HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(dac->spiHandle, dac->buffer, 3, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_PORT, CS_GPIO_PIN, GPIO_PIN_SET);
    return status;
}

/**
 * DAC8532_Output_Voltage(DAC8532 *dac, uint8_t command, float voltage)
 * 
 * Output a specific analog voltage between DAC_VREF volts and 0.0 volts
 * from the DAC8532 initialized by *dac using the provided 8 bit 
 * command. There are 4 acceptable 8 bit commands:
 *      - LOAD_CHANNEL_A    (0x10): Write to data buffer A and load DAC A
 *      - LOAD_CHANNEL_B    (0x24): Write to data buffer B and load DAC B
 *      - WRITE_CHANNEL_A   (0x00): Write to data buffer A (no output on VoutA)
 *      - WRITE_CHANNEL_B   (0x04): Write to data buffer B (no output on VoutB)
 * 
 * The above commands may be OR'd together, for example:
 *      (LOAD_CHANNEL_A | LOAD_CHANNEL_B)
 *      Write to data buffer B and load DAC A and DAC B simultaneously.
 * 
 * Returns a HAL_StatusTypeDef indicating the success of the operation.
*/
HAL_StatusTypeDef DAC8532_Output_Voltage(DAC8532 *dac, uint8_t command, float voltage)
{
    if (voltage <= DAC_VREF && voltage >= 0.0)
    {
        uint16_t data = (uint16_t) (voltage / DAC_VREF * DAC_MAX_VALUE);
        return DAC8532_Write_Data(dac, command, data);
    }
    return HAL_ERROR;
}
