 /*
 * peripheral_Init.h
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

#ifndef INC_PERIPHERAL_INIT_H_
#define INC_PERIPHERAL_INIT_H_

// general purpose library for this program
#include "stm32l4xx_hal.h"

// library for various types of integers
#include <stdint.h>

// library for string manipulation
#include <string.h>

// library containing global variables
#include "global_variables.h"

/* Macros */

#define COVERSION_TO_TIME_SCALE ((uint16_t) 7999)
#define COUNTING_TO_SECOND ((uint16_t) 10000)
#define COUNTING_TO_5_SECONDS ((uint16_t) 50000)
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* Functions used for initialising various peripherals */

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_SPI1_Init(void);
void MX_TIM7_Init(void);

/* General purpose functions */

#endif /* INC_PERIPHERAL_INIT_H_ */
