/*
 * global_variables.h
 *
 *  Created on: Aug 28, 2024
 *      Author: Budi
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_

/* -------------------- Macros -------------------- */

/* SPI communication specific macros */

#define SPI_Rx_Buffer_Size ((uint8_t) 50)
#define SPI_Tx_Buffer_Size ((uint8_t) 50)

/* Terminal communication specific macros */

#define Rx_Buffer_Size ((uint8_t) 50)
#define Tx_Buffer_Size ((uint8_t) 100)

#define Rx_LoRa_Buffer_Size ((uint8_t) 50)
#define Tx_LoRa_Buffer_Size ((uint8_t) 100)



/* --------------- Global variables --------------- */

/* SPI communication variables */

// buffers, iterators, counters and flags

// SPI1 Receive Buffer
extern int8_t SPI_Rx_Buffer[SPI_Rx_Buffer_Size];

// SPI1 Receive Buffer Iterator
extern uint8_t SPI_Rx_Buffer_Iter;

// SPI1 Transmit Buffer
extern int8_t SPI_Tx_Buffer[SPI_Tx_Buffer_Size];

// SPI1 Transmit Buffer Iterator
extern uint8_t SPI_Tx_Buffer_Iter;

/* Terminal communication variables */

// buffers, iterators, counters and flags

// USART2 Receive Buffer
extern int8_t Rx_Buffer[Rx_Buffer_Size];

// USART2 Receive Buffer Iterator
extern uint8_t Rx_Buffer_Iter;

// USART2 Transmit Buffer
extern int8_t Tx_Buffer[Tx_Buffer_Size];

// USART2 Transmit Buffer Iterator
extern uint8_t Tx_Buffer_Iter;

// USART2 Receive Buffer for the event of LoRa reception
extern int8_t Rx_LoRa_Buffer[Rx_LoRa_Buffer_Size];

// USART2 Receive Buffer Iterator for the event of LoRa reception
extern uint8_t Rx_LoRa_Buffer_Iter;

// USART2 Transmit Buffer for the event of LoRa reception
extern int8_t Tx_LoRa_Buffer[Tx_LoRa_Buffer_Size];

// USART2 Transmit Buffer Iterator used for the event of LoRa reception
extern uint8_t Tx_LoRa_Buffer_Iter;

// number of messages sent from the terminal
extern uint16_t nr_of_messages;

// flag used for direct SPI communication with the LoRa board
extern uint8_t SPI_Message_Flag;

// flag used for sending a message through LoRa
extern uint8_t LoRa_Tx_Message_Flag;

// flag used for sending a message through LoRa
extern uint8_t LoRa_Rx_Message_Flag;

// flag used for signifying that we should introduce the number of bytes to be received through SPI
extern uint8_t Nr_Of_Messages_Flag;

/* Peripheral handle structures */

// handle structure for the UART peripheral
extern UART_HandleTypeDef huart2;

// handle structure for the SPI peripheral
extern SPI_HandleTypeDef hspi1;

// handle structure for the timer peripherals
extern TIM_HandleTypeDef htim7;

/* GPIO interrupts variables */

extern uint8_t Irq_Flag;

#endif /* INC_GLOBAL_VARIABLES_H_ */
