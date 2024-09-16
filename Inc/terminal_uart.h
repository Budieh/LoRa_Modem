/*
 * terminal_uart.h
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

#ifndef INC_TERMINAL_UART_H_
#define INC_TERMINAL_UART_H_

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

/* Functions used for UART communication with the terminal */

// callback used for reception completion on UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

// function used for writing a message to the Tx_Buffer
void WritingMessageToTerminal(UART_HandleTypeDef *huart, int8_t * buffer);

// function used for reading a byte from the terminal
void ReadingByteFromTerminal(UART_HandleTypeDef *huart);

// function used for continuous receive action through LoRa
uint8_t ContinuousLoRaReceive(UART_HandleTypeDef *huart);

#endif /* INC_TERMINAL_UART_H_ */
