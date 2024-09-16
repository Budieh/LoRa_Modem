// library that includes all the necessary resources for the program
#include "main.h"

int main(void)
{
  /* Reset of all peripherals, Initialises the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialise all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM7_Init();

  // initial restart of the LoRa board
  Initial_Restart(&htim7);

  // set the pin connected to the NRESET pin on the LoRa board to set state
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
  // set the pin connected to the NSS pin on the LoRa board to set state
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  // starting the communication with the terminal through UART in interrupt mode
  ReadingByteFromTerminal(&huart2);

  // infinite loop
  while (1)
  {
      uint8_t flag = ContinuousLoRaReceive(&huart2);

      if(flag)
      {
    	  sprintf(Tx_Buffer,"\r\nLoRa reception done.\r\n");
          WritingMessageToTerminal(&huart2, Tx_Buffer);
      }
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
