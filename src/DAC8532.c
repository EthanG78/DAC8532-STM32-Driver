/*
*   DAC8532 STM32F7 SPI Driver
*   
*   Author:     Ethan Garnier
*   Date:       May 2nd, 2024
*/
#include "DAC8532.h"

/**
 * DAC8532_Init(DAC8532 *dac, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
 * 
 * Initialize the passed DAC8532 struct with the provided STM32 SPI handler 
 * and send some initial dummy data (0x0000) to ensure SPI connection is working
 * 
 * Returns a HAL_StatusTypeDef indiciating the success of the operation.
*/
HAL_StatusTypeDef DAC8532_Init(DAC8532 *dac, SPI_HandleTypeDef *spiHandle, GPIO_TypeDef *csPort, uint16_t csPin)
{
    HAL_StatusTypeDef status;

    dac->spiHandle = spiHandle;
    dac->buffer[0] = 0x00;
    dac->buffer[1] = 0x00;
    dac->buffer[2] = 0x00;
    dac->csPort = csPort;
    dac->csPin = csPin;

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
__attribute__((optimize("-Ofast"))) HAL_StatusTypeDef DAC8532_Write_Data(DAC8532 *dac, uint8_t command, uint16_t data)
{
	// Populate the buffer of data to be sent
    dac->buffer[0] = command;
    dac->buffer[1] = data >> 8;
    dac->buffer[2] = data & 0xFF;

    // Grab the base address of the buffer
	uint8_t *bufferAddr = dac->buffer;

    // Bring the DAC chip select pin low
    // by setting BR for pin number
    dac->csPort->BSRR = (uint32_t)dac->csPin << 16;

    // Enable SPI by setting SPE bit (bit 6)
    dac->spiHandle->Instance->CR1 |= 0x0040;

    // Send 3 bytes of data (command + data) to
    // SPI Tx buffer via the data register
    uint8_t nBytes = 3;
    while (nBytes > 0)
    {
    	// Check if Tx buffer is empty (bit 1 of status register is set)
    	if ((dac->spiHandle->Instance->SR & 0x0002) == 0x0002)
    	{
    		// dac->spiHandle->Instance->DR = dac->buffer[--nBytes];
    		if (nBytes > 1)
    		{
    			// write on the data register in packing mode
    			dac->spiHandle->Instance->DR = *((uint16_t *)bufferAddr);
    			bufferAddr += sizeof(uint16_t);
    			nBytes -= 2;
    		}
    		else
    		{
    			*((volatile uint8_t *)&dac->spiHandle->Instance->DR) = (*bufferAddr);
    			bufferAddr++;
    			nBytes--;
    		}
    	}
    }

    // Disable SPI using procedure outlined in
    // Section 32.5.9 of Reference Manual 0385:
    // Wait until FTLVL[1:0] is 0b00 and BSY is 0
    while ((dac->spiHandle->Instance->SR & 0x1800) == 0x1800
    		|| (dac->spiHandle->Instance->SR & 0x0080) == 0x0080)
    	{};

    // Enable SPI by clearing SPE bit (bit 6)
    dac->spiHandle->Instance->CR1 &= 0xFFBF;

    // Bring the DAC chip select pin high
    // by setting BS for pin number
    dac->csPort->BSRR = dac->csPin;

    return HAL_OK;
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
