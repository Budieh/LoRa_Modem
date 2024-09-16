/*
 * stm32l4xx_it.h
 *
 * Created on: Sep 05, 2024
 * Auto-generated library containing interrupt service routines declarations,
 * modified by user
 */

#ifndef __STM32L4xx_IT_H
#define __STM32L4xx_IT_H

// general-purpose library containing includes for the main function
#include "main.h"

// library containing global variables
#include "global_variables.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*--------------------------Exported functions prototypes--------------------------*/

 /*-------------System Interrupt Service Routine Functions Declarations-------------*/

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/*-----------Peripheral Interrupt Service Routine Functions Declarations-----------*/

void SPI1_IRQHandler(void);
void USART2_IRQHandler(void);
void TIM7_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
