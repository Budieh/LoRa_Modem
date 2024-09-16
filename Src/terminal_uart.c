/*
 * terminal_uart.c
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

// parent library
#include "terminal_uart.h"

/* Global variables */

// buffers, iterators, counters and flags

// USART2 Receive Buffer
int8_t Rx_Buffer[Rx_Buffer_Size];

// USART2 Receive Buffer Iterator
uint8_t Rx_Buffer_Iter;

// USART2 Transmit Buffer
int8_t Tx_Buffer[Tx_Buffer_Size];

// USART2 Transmit Buffer Iterator
uint8_t Tx_Buffer_Iter;

// USART2 Receive Buffer for the event of LoRa reception
int8_t Rx_LoRa_Buffer[Rx_LoRa_Buffer_Size];

// USART2 Receive Buffer Iterator for the event of LoRa reception
uint8_t Rx_LoRa_Buffer_Iter;

// USART2 Transmit Buffer for the event of LoRa reception
int8_t Tx_LoRa_Buffer[Tx_LoRa_Buffer_Size];

// USART2 Transmit Buffer Iterator used for the event of LoRa reception
uint8_t Tx_LoRa_Buffer_Iter;

// number of messages sent from the terminal
uint16_t nr_of_messages;

// flag used for direct SPI communication with the LoRa board
uint8_t SPI_Message_Flag;

// flag used for sending a message through LoRa
uint8_t LoRa_Tx_Message_Flag;

// flag used for sending a message through LoRa
uint8_t LoRa_Rx_Message_Flag;

// flag used for signifying that we should introduce the number of bytes to be received through SPI
uint8_t Nr_Of_Messages_Flag;

/* External functions */

extern uint8_t LoRa_Data_Byte_Tx(int8_t data, const uint8_t bypass_LoRa_Tx_Message_Flag);
extern uint8_t LoRa_Data_Byte_Rx(int8_t * data, const uint8_t bypass_LoRa_Tx_Message_Flag);
extern void SendAndReceiveCommand(SPI_HandleTypeDef * hspi, int8_t * command, uint8_t command_length, uint8_t has_response, int8_t * response, uint8_t response_length);

extern void Start_Reset_Timer(TIM_HandleTypeDef *htim);

/* General purpose function declarations */

// function called in case of failure while operating with a peripheral
static void Error_Handler(void);

// function used for converting alpha-numerical hex values to bytes
static void HexToByte(void);

// function used for converting alpha-numerical decimal values to bytes
static uint8_t DecimalToByte(void);

/* Functions used for UART communication with the terminal */

// callback used for reception completion on UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// USART2 is the UART peripheral used for communication with the terminal
	if(huart->Instance == USART2)
	{
	  // in case we aren't interacting with the LoRa board
      if(!(SPI_Message_Flag || LoRa_Tx_Message_Flag || LoRa_Rx_Message_Flag || Nr_Of_Messages_Flag) )
      {
         // switch case used for processing the characters sent from the terminal
         switch(Rx_Buffer[Rx_Buffer_Iter - 1])
         {
         // in case we reached the end of the message ( the last character in the
	     // Rx_Buffer is '\r' ), we loop it back to the terminal and reset the iterator
         case '\r':

        	 if(Rx_Buffer_Iter != 0)
        	 {
        	    // send the message to the terminal through UART
        	 	if(HAL_UART_Transmit(huart,Rx_Buffer,Rx_Buffer_Iter,HAL_MAX_DELAY) != HAL_OK)
        	    {
        	 	   Error_Handler();
        	 	}
        	 }

		    // remember to transmit the number of messages sent to the terminal
		    sprintf(Tx_Buffer,"Number of sent messages: %d\r\n", ++nr_of_messages);
		    WritingMessageToTerminal(huart, Tx_Buffer);

		    // reset the Rx_Buffer_Iterator
		    Rx_Buffer_Iter = 0;

         break;

	     // if the backspace character is introduced, decrement the iterator ( if the
         // iterator is not on the first position of the buffer
         case 0x7F:
    	    if(Rx_Buffer_Iter > 1) Rx_Buffer_Iter -= 2;
         break;

	     // the '%' character signifies that the message sent from the terminal is
	     // meant to be sent to the LoRa module through SPI ( and reset the iterator
         // afterwards )
         // the format of this message is as follows: first byte represents the opcode of the operation
         // performed on the LoRa module and from here on the rest of the bytes until enter is pressed
         // represent the parameters passed to the function corresponding to the opcode mentioned above


         // the '?' character is used for providing information about the LoRa
         // module
         case '?':

            sprintf(Tx_Buffer,"\r\nSX1261 board specifications:\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Modulation type: LoRa\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Modulation parameters:\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Spreading factor: SP7\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Bandwidth: 500 kHz\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Coding rate: 4/5\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Low data rate optimisation: off\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Frequency: 868.5 MHz\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Transmission Power: 0 dBm\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Ramp Up Time: 200 us\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Transmission timeout: continuous\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Reception timeout: 5 seconds\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"Packet parameters:\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Preamble length: 16 bits\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Header: explicit\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   Payload length: 1 byte\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   CRC: disabled\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"   IQ: standard setup\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);
            sprintf(Tx_Buffer,"\r\nYou can proceed using the LoRa board.\r\n");
            WritingMessageToTerminal(huart, Tx_Buffer);

            // reset the Rx_Buffer_Iter
            Rx_Buffer_Iter = 0;

         break;

         case '$':

    	    // marking the beginning of an SPI transaction
            if( SPI_Message_Flag == 0 )
            {
               // setting the message flag
        	   SPI_Message_Flag = 1;
        	   // reset Rx_Buffer_Iter
        	   Rx_Buffer_Iter = 0;

        	   //sending a fitting message to the terminal
               sprintf(Tx_Buffer,"\r\nSPI message initiated. Introduce opcode and parameters in this order: ");
               WritingMessageToTerminal(huart, Tx_Buffer);
            }
         break;

         // the '^' character is used to toggle the pin that resets the LoRa board. Sending this character resets
         // all iterators and starts a timer of 1 second that restarts the board once once it's pressed

         case '^':

       	    // start the timer that sets the pin connected to the NRESET pin on the LoRa board back to SET state
       	    // after 1 second

    	    Start_Reset_Timer(&htim7);

         break;

         // initiation of a transmission through LoRa
         case '>':

    	    // marking the beginning of a message that's going to be sent through LoRa
            if( LoRa_Tx_Message_Flag == 0 )
            {
        	   // setting the message flag
        	   LoRa_Tx_Message_Flag = 1;
        	   // reset Rx_Buffer_Iter
        	   Rx_Buffer_Iter = 0;

        	   //sending a fitting message to the terminal
               sprintf(Tx_Buffer,"\r\nLoRa transmission initiated.\r\nWrite your desired message and hit enter to send: \r\n");
               WritingMessageToTerminal(huart, Tx_Buffer);
            }

         break;

         }

         // also reset the iterator in case the message exceeds Rx_Buffer_Size bytes ( and
	     // send a fitting message to the terminal so that we know an overflow occurred )

	     if(Rx_Buffer_Iter == Rx_Buffer_Size - 1)
	     {
		    // message sent to the terminal so we know the overflow occurred
		    sprintf(Rx_Buffer,"\r\nOverflow of the input buffer. Start over. \r\n");
		    WritingMessageToTerminal(huart, Tx_Buffer);

		    // remember to transmit the number of messages sent to the terminal
		    sprintf(Rx_Buffer,"Number of sent messages: %d\r\n", ++nr_of_messages);
		    WritingMessageToTerminal(huart, Tx_Buffer);

		    // reset the Rx_Buffer_Iter
		    Rx_Buffer_Iter = 0;
	     }

	     ReadingByteFromTerminal(huart);

	   }


	   // in case we are transmitting a message through LoRa
	   else if( LoRa_Tx_Message_Flag )
	   {
	   	  // switch case used for processing the characters sent from the terminal
		  switch(Rx_Buffer[Rx_Buffer_Iter - 1])
		  {
		  // in case we reached the end of the message ( the last character in the
		  // Rx_Buffer is '\r' ), we send it through LoRa
		  case '\r':

			 // iterator for the Tx_Message
			 uint8_t i;

		     for(i = 0; i < Rx_Buffer_Iter; i++)
		     {
                // pin 1 of port C is connected to the led signifying LoRa transmission
		    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

		    	if(!LoRa_Data_Byte_Tx(Rx_Buffer[i],0x01))
		        {
		    	   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		           break;
		        }

		    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

		        // looping back the sent byte to ensure everything worked according to plan
		        int8_t data;
		        uint8_t nr_of_trials = 5;
                uint8_t Rx_Failure_Flag = 0;

		        if(!LoRa_Data_Byte_Rx(&data,0x01))
		        {
		        	Rx_Failure_Flag = 1;
		        }
		        while((!Rx_Failure_Flag) && (data != 127 - Rx_Buffer[i]) && ( 0 < nr_of_trials--))
		        {
		        	if(!LoRa_Data_Byte_Rx(&data,0x01))
		            {
		        	   Rx_Failure_Flag = 1;
		        	   break;
		            }
		        }

		        if((!Rx_Failure_Flag) && nr_of_trials != 0 )
		        {
		        	//sending a fitting message to the terminal
		            sprintf(Tx_Buffer,"\r\nByte %d sent successfully\r\n", i+1);
		            WritingMessageToTerminal(huart, Tx_Buffer);
		        }

		        else
		        {
		        	//sending a fitting message to the terminal
		            sprintf(Tx_Buffer,"\r\nByte %d transmission failed! Stopping transmission\r\n", i+1);
		            WritingMessageToTerminal(huart, Tx_Buffer);
		            break;
		        }
		     }

		     //sending a fitting message to the terminal
		     if( i == Rx_Buffer_Iter && Rx_Buffer_Iter > 0)
		     {
		    	sprintf(Tx_Buffer,"\r\nLoRa message sent. Length: %d characters\r\n", Rx_Buffer_Iter);
		        WritingMessageToTerminal(huart, Tx_Buffer);
		     }

		     else if( Rx_Buffer_Iter > 0)
		     {
		    	 sprintf(Tx_Buffer,"\r\nLoRa transmission failed. Please reset the LoRa board\r\n");
		    	 WritingMessageToTerminal(huart, Tx_Buffer);
		     }

		     // reset the Rx_Buffer_Iter
		     Rx_Buffer_Iter = 0;

		     // clear the LoRa_Tx flag
		     LoRa_Tx_Message_Flag = 0;

		  break;

		  // if the backspace character is introduced, decrement the iterator ( if the
		  // iterator is not on the first position of the buffer
		  case 0x7F:
		     if(Rx_Buffer_Iter > 1) Rx_Buffer_Iter -= 2;
		  break;

		  // the '~' character cancels the transaction
		  case '~':

		     //sending a fitting message to the terminal
			 sprintf(Tx_Buffer,"\r\nLoRa transmission aborted.\r\n");
			 WritingMessageToTerminal(huart, Tx_Buffer);

			 // reset the Rx_Buffer_Iter
			 Rx_Buffer_Iter = 0;

			 // clear the LoRa_Tx flag
			 LoRa_Tx_Message_Flag = 0;

	      break;

	      }

	      if(Rx_Buffer_Iter == Rx_Buffer_Size - 1)
	      {

		     // message sent to the terminal so we know the overflow occurred
		     sprintf(Tx_Buffer,"\r\nOverflow of the input buffer. Start over. \r\n");
   		     WritingMessageToTerminal(huart, Tx_Buffer);

		     // reset the Rx_Buffer_Iter
		     Rx_Buffer_Iter = 0;

		  }

		  ReadingByteFromTerminal(huart);

	   }

       // in case we are transmitting a message to the LoRa board
       else if( SPI_Message_Flag )
       {
          // switch case used for processing the characters sent from the terminal
      	  switch(Rx_Buffer[Rx_Buffer_Iter - 1])
      	  {
      	     // in case we reached the end of the message ( the last character in the
      		 // Rx_Buffer is '\r' ), we send it to the LoRa board
      		 case '\r':

      			// processing the message ( turning Hex values to decimal values )
      		    HexToByte();

      			//asking the user how many bytes should be received from the LoRa board through SPI
      			sprintf(Tx_Buffer,"Introduce the number of bytes to be received from the LoRa board: ");
      			WritingMessageToTerminal(huart, Tx_Buffer);

      			// set the Nr_Of_Messages_Flag
      			Nr_Of_Messages_Flag = 1;

      			// reset the SPI_Tx_Message_Flag
      			SPI_Message_Flag = 0;

      		 break;

      		 // if the backspace character is introduced, decrement the iterator ( if the
      		 // iterator is not on the first position of the buffer
      		 case 0x7F:
      		    if(Rx_Buffer_Iter > 1) Rx_Buffer_Iter -= 2;
      		 break;

      		 // the '~' character cancels the transaction
      		 case '~':

      		    //sending a fitting message to the terminal
      			sprintf(Tx_Buffer,"\r\nSPI transmission aborted.\r\n");
      			WritingMessageToTerminal(huart, Tx_Buffer);

      			// reset the Rx_Buffer_Iter
      			Rx_Buffer_Iter = 0;

      			// clear the SPI_Message_flag
      			SPI_Message_Flag = 0;

      	     break;

      	  }

      	  if(Rx_Buffer_Iter == Rx_Buffer_Size - 1)
      	  {

      		 // message sent to the terminal so we know the overflow occurred
      		 sprintf(Tx_Buffer,"\r\nOverflow of the input buffer. Start over. \r\n");
             WritingMessageToTerminal(huart, Tx_Buffer);

      		 // reset the Rx_Buffer_Iter
      		 Rx_Buffer_Iter = 0;

          }

      	  ReadingByteFromTerminal(huart);

       }

       // in case we are processing how many data bytes we should be receiving from the LoRa board
       else if( Nr_Of_Messages_Flag )
       {
          // switch case used for processing the characters sent from the terminal
          switch(Rx_Buffer[Rx_Buffer_Iter - 1])
          {
             // in case we reached the end of the message ( the last character in the
             // Rx_Buffer is '\r' ), we send it to the LoRa board
             case '\r':

                // processing the message ( turning Hex values to decimal values )
                uint8_t nr_of_messages = DecimalToByte();
                SPI_Rx_Buffer_Iter = 1;

                // sending the message to the LoRa board through SPI and receiving nr_of_messages bytes
                SendAndReceiveCommand(&hspi1, SPI_Tx_Buffer,SPI_Tx_Buffer_Iter, 0x01, SPI_Rx_Buffer, nr_of_messages);

            	// showing the received data bytes
                sprintf(Tx_Buffer,"Received data bytes: ");
                WritingMessageToTerminal(huart, Tx_Buffer);

                for(uint8_t i = 0; i < SPI_Rx_Buffer_Iter; i++ )
                {
                   sprintf(Tx_Buffer,"%x ", SPI_Rx_Buffer[i]);
                   WritingMessageToTerminal(huart, Tx_Buffer);
                }

                sprintf(Tx_Buffer,"\r\n");
                WritingMessageToTerminal(huart, Tx_Buffer);

                // reset the Nr_Of_Messages_Flag
                Nr_Of_Messages_Flag = 0;

                // reset both SPI_Rx_Buffer_Iter and SPI_Tx_Buffer_Iter

                SPI_Rx_Buffer_Iter = 0;
                SPI_Tx_Buffer_Iter = 0;
             break;

             // if the backspace character is introduced, decrement the iterator ( if the
             // iterator is not on the first position of the buffer
             case 0x7F:
                if(Rx_Buffer_Iter > 1) Rx_Buffer_Iter -= 2;
             break;

             // the '~' character cancels the transaction
             case '~':

                //sending a fitting message to the terminal
            	sprintf(Tx_Buffer,"\r\nSPI transmission aborted.\r\n");
            	WritingMessageToTerminal(huart, Tx_Buffer);

            	// reset the Rx_Buffer_Iter
            	Rx_Buffer_Iter = 0;

            	// clear the Nr_Of_Messages_Flag
                Nr_Of_Messages_Flag = 0;

             break;

             }

          if(Rx_Buffer_Iter == Rx_Buffer_Size - 1)
          {

             // message sent to the terminal so we know the overflow occurred
             sprintf(Tx_Buffer,"\r\nOverflow of the input buffer. Start over. \r\n");
             WritingMessageToTerminal(huart, Tx_Buffer);

             // reset the SPI_Tx_Buffer_Iter
             Rx_Buffer_Iter = 0;

          }

          ReadingByteFromTerminal(huart);

       }
	}
}

// function used for writing a message to the terminal
void WritingMessageToTerminal(UART_HandleTypeDef *huart, int8_t * buffer)
{
	uint8_t buffer_length = strlen(buffer);
	if(buffer_length != 0)
	{
	   // send the message to the terminal through UART
	   if(HAL_UART_Transmit(huart,buffer,buffer_length,HAL_MAX_DELAY) != HAL_OK)
       {
	      Error_Handler();
	   }
	}

	// clear the Tx_Buffer and reset the Tx_Buffer_Iter
	Tx_Buffer_Iter = 0;
}

// function used for reading a byte from the terminal
void ReadingByteFromTerminal(UART_HandleTypeDef *huart)
{
	// restart receiving data from the terminal through UART
    if(HAL_UART_Receive_IT(huart,Rx_Buffer + Rx_Buffer_Iter ++, 0x01) != HAL_OK)
    {
	   //Error_Handler();
    }
}

// function used for continuous receive action through LoRa
uint8_t ContinuousLoRaReceive(UART_HandleTypeDef *huart)
{
   beginning:

   // resetting SPI_Rx_Buffer_Iter
   Rx_LoRa_Buffer_Iter = 0;

   // keep receiving bytes of the message through LoRa until timeout or a full message received
   while(( ( Rx_LoRa_Buffer_Iter == 0 ) || ( Rx_LoRa_Buffer[Rx_LoRa_Buffer_Iter - 1] != '\r' ) ) && (!LoRa_Tx_Message_Flag))
   {
	  // pin 0 of port C is connected to the led signifying LoRa reception
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

      // in case an error occurred during LoRa reception stop the reception and send a fitting messages
      if((!LoRa_Data_Byte_Rx(Rx_LoRa_Buffer + Rx_LoRa_Buffer_Iter,0x00)) && (!LoRa_Tx_Message_Flag))
      {
    	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

          // resetting SPI_Rx_Buffer_Iter
          Rx_LoRa_Buffer_Iter = 0;

          break;
      }

      // if the reception is successful, increment SPI_Rx_Buffer_Iter and send the received message back to the transmitter
      else if(!LoRa_Tx_Message_Flag)
      {
    	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

    	  if( Rx_LoRa_Buffer[Rx_LoRa_Buffer_Iter] >= 0x00 )
    	  {
			LoRa_Data_Byte_Tx(127 - Rx_LoRa_Buffer[Rx_LoRa_Buffer_Iter],0x00);

			if( Rx_LoRa_Buffer_Iter < Rx_LoRa_Buffer_Size )
			{
			  Rx_LoRa_Buffer_Iter ++;
			}

			else
			{
			   Rx_LoRa_Buffer_Iter = 0;
			}
    	  }
      }

      else if(LoRa_Tx_Message_Flag)
      {
    	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
    	  return 0;
      }

   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
   }

   // send the received message to the terminal
   if( (Rx_LoRa_Buffer[Rx_LoRa_Buffer_Iter - 1] == '\r' && Rx_LoRa_Buffer_Iter >= 2) && (!LoRa_Tx_Message_Flag))
   {
      memset(Tx_LoRa_Buffer,0, strlen(Tx_LoRa_Buffer));
      strncpy(Tx_LoRa_Buffer, Rx_LoRa_Buffer, Rx_LoRa_Buffer_Iter);

      // resetting SPI_Rx_Buffer_Iter
      Rx_LoRa_Buffer_Iter = 0;

      WritingMessageToTerminal(huart, Tx_LoRa_Buffer);

      return 1;

   }

   else if(LoRa_Tx_Message_Flag)
   {
	  return 0;
   }

   else
   {
      goto beginning;
   }
}

/* General purpose functions */

// function used for converting alpha-numerical hex values to bytes
static void HexToByte(void)
{
   // reset SPI_Tx_Buffer
   SPI_Tx_Buffer_Iter = 0;

   // processing the message so we can send it through SPI in the form of bytes
   for(uint8_t i = 0; i < Rx_Buffer_Iter; i+=3)
   {
      if(Rx_Buffer[i] < 65) SPI_Tx_Buffer[SPI_Tx_Buffer_Iter] = (Rx_Buffer[i] - 48) << 4;
	  else SPI_Tx_Buffer[SPI_Tx_Buffer_Iter] += (Rx_Buffer[i] - 55) << 4;

	  if(Rx_Buffer[i+1] < 65) SPI_Tx_Buffer[SPI_Tx_Buffer_Iter++] += Rx_Buffer[i+1] - 48;
	  else SPI_Tx_Buffer[SPI_Tx_Buffer_Iter++] += Rx_Buffer[i+1] - 55;
   }

   // reset SPI_Tx_Buffer
   Rx_Buffer_Iter = 0;
}

// function used for converting alpha-numerical decimal values to bytes
static uint8_t DecimalToByte(void)
{
   uint8_t nr_of_messages = 0;
   for(uint8_t i = 0; i < Rx_Buffer_Iter - 1; i++)
   {
	   nr_of_messages = 10 * nr_of_messages + Rx_Buffer[i] - 48;
   }

   // reset the Rx_Buffer_Iter
   Rx_Buffer_Iter = 0;

   return nr_of_messages;
}

// function called in case of failure while operating with a peripheral
static void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
