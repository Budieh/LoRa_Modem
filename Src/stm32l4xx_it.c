/*
 * stm32l4xx_it.c
 *
 * Created on: Sep 05, 2024
 * Auto-generated source file containing interrupt service routines implementations,
 * modified by user
 */

// parent library
#include "stm32l4xx_it.h"

/*-------------System Interrupt Service Routine Functions Implementations-------------*/

// This function handles Non maskable interrupt.
void NMI_Handler(void)
{
  while (1)
  {
  }
}

// This function handles Hard fault interrupt.
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

// This function handles Memory management fault.
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

// This function handles Prefetch fault, memory access fault.
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

// This function handles Undefined instruction or illegal state.
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

// This function handles System service call via SWI instruction.
void SVC_Handler(void)
{
}

// This function handles Debug monitor.
void DebugMon_Handler(void)
{
}

// This function handles Pendable request for system service.
void PendSV_Handler(void)
{
}

// This function handles System tick timer.
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/*-----------Peripheral Interrupt Service Routine Functions Implementations-----------*/

// This function handles SPI1 global interrupt.
void SPI1_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi1);
}

// This function handles USART2 global interrupt.
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

// This function handles TIM7 global interrupt.
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}
