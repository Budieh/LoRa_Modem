/*
 * reset_timer.h
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

#ifndef INC_RESET_TIMER_H_
#define INC_RESET_TIMER_H_

// general purpose library for this program
#include "stm32l4xx_hal.h"

// library for various types of integers
#include <stdint.h>

// library for string manipulation
#include <string.h>

// library used mainly for the inclusion of the 'sprintf' function
#include <stdio.h>

// library containing global variables
#include "global_variables.h"

/* Functions */

// callback that gets called when the timer used for reseting the LoRa board is called
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

// function used for starting the reset timer
void Start_Reset_Timer(TIM_HandleTypeDef *htim);

// initial restart action of the LoRa board used at beginning of the program
void Initial_Restart(TIM_HandleTypeDef *htim);

#endif /* INC_RESET_TIMER_H_ */
