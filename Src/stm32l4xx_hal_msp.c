/*
 * stm32l4xx_hal_msp.c
 *
 * Created on: Sep 05, 2024
 * Auto-generated source file containing MCU support package init and deinit
 * functions, modified by user
 */

// general-purpose library containing includes for the main function
#include "main.h"

/*----------------------General purpose function declarations----------------------*/

// function called in case of failure while operating with a peripheral
static void Error_Handler(void);

/*--------------------Init and Deinit functions implementations--------------------*/

// Function used for initialising the Global MSP.
void HAL_MspInit(void)
{
  // Peripheral clock enable
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

/*
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI1)
  {
	// Peripheral clock enable
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */

    // GPIO init
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Interrupt init
    HAL_NVIC_SetPriority(SPI1_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  }

}

/*
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
	// Peripheral clock disable
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */

    // GPIO deinit
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    // Interrupt deinit
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  }

}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance == TIM7)
  {
	// Peripheral clock enable
    __HAL_RCC_TIM7_CLK_ENABLE();

    // Interrupt init
    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
}

/*
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance == TIM7)
  {
	// Peripheral clock disable
    __HAL_RCC_TIM7_CLK_DISABLE();

    // Interrupt deinit
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  }

}

/*
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(huart->Instance==USART2)
  {
    // Peripheral clock configuration
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    // Peripheral clock enable
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */

    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Interrupt init
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }

}

/*
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
	// Peripheral clock disable
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */

    // GPIO disable
    HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

    // Interrupt deinit
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }

}

/*-------------------General purpose function implementations-------------------*/

// function called in case of failure while operating with a peripheral
static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
