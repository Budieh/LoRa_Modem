/*
 * LoRa_SPI.h
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

#ifndef INC_LORA_SPI_H_
#define INC_LORA_SPI_H_

// general purpose library for this board
#include "stm32l4xx_hal.h"

// library for various types of integers
#include <stdint.h>

// library for string manipulation
#include <string.h>

// library used mainly for the inclusion of the 'sprintf' function
#include <stdio.h>

// library containing global variables
#include "global_variables.h"

/* Functions used for SPI communication with the LoRa board */

// callback that gets called after the SPI transmit transaction is over
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

// callback that gets called when the SPI receive transaction is over
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

// function used for starting receiving data bytes from LoRa through SPI in interrupt mode
void ReadingByteFromLoRa(SPI_HandleTypeDef *hspi);

// function used for sending 1 data byte to LoRa board through SPI in interrupt mode
void WritingMessageToLoRa(SPI_HandleTypeDef *hspi);

// function used for sending a command through SPI to the LoRa board
void SendAndReceiveCommand(SPI_HandleTypeDef * hspi, int8_t * command, uint8_t command_length, uint8_t has_response, int8_t * response, uint8_t response_length);

// function used for sending 1 byte of data through LoRa
uint8_t LoRa_Data_Byte_Tx(int8_t data, const uint8_t bypass_LoRa_Tx_Message_Flag);

// function used for receiving 1 byte of data through LoRa
uint8_t LoRa_Data_Byte_Rx(int8_t * data, const uint8_t bypass_LoRa_Tx_Message_Flag);

#endif /* INC_LORA_SPI_H_ */
