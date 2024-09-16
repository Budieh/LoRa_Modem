/*
 * reset_timer.c
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

// parent library
#include "reset_timer.h"

/* Global variables */

/* External functions */
extern void WritingMessageToTerminal(UART_HandleTypeDef *huart, int8_t * buffer);

/* General purpose functions */

static void Error_Handler(void);

/* Functions */

// callback that gets called when the timer used for reseting the LoRa board is called
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// static variable used as a flag for the initial restart sequence
	static uint8_t initial_restart_tim7 = 0;

	if(htim->Instance == TIM7)
	{
		// stop the timer
		HAL_TIM_Base_Stop_IT(htim);

		// send a fitting message to the terminal ( only in the case of a user initiated restart, not the initial one )
		if(initial_restart_tim7 != 0)
		{
	       sprintf(Tx_Buffer,"\r\nLoRa board successfully restarted.\r\nYou can start sending messages through SPI\r\n");
		   WritingMessageToTerminal(&huart2, Tx_Buffer);

		}

		// otherwise, set the initial_restart flag
		else
		{
			initial_restart_tim7 = 1;
		}

		LoRa_Tx_Message_Flag = 0;

		// set the pin connected to the NRESET pin on the LoRa board to set state
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	}
}

// function used for starting the reset timer
void Start_Reset_Timer(TIM_HandleTypeDef *htim)
{
   if(htim->Instance == TIM7)
   {

	  // set LoRa_Tx_Message Flag so all other actions are stopped
      LoRa_Tx_Message_Flag = 1;

	  // set the pin connected to the NRESET pin on the LoRa board to reset state
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

	  // reset all buffer iterators
	  Rx_Buffer_Iter = 0;
	  Tx_Buffer_Iter = 0;
	  SPI_Rx_Buffer_Iter = 0;
	  SPI_Tx_Buffer_Iter = 0;

	  // send a fitting message to the terminal
	  sprintf(Tx_Buffer,"\r\nLoRa board restart sequence initiated\r\nWait 1 second\r\n");
	  WritingMessageToTerminal(&huart2, Tx_Buffer);

	  // start the timer
	  if( HAL_TIM_Base_Start_IT(htim) != HAL_OK )
	  {
		  Error_Handler();
	  }
   }
}

// initial restart action of the LoRa board used at beginning of the program
void Initial_Restart(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM7)
  {
	// set the pin connected to the NRESET pin on the LoRa board to reset state
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

    // start the timer
    if( HAL_TIM_Base_Start_IT(htim) != HAL_OK )
    {
       Error_Handler();
    }
  }
}

/* General purpose functions */

// function called in case of failure while operating with a peripheral
static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}







