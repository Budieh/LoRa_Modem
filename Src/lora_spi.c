/*
 * lora_spi.c
 *
 *  Created on: Aug 27, 2024
 *      Author: Budi
 */

// parent library
#include "lora_spi.h"

/* Global variables */

// buffers, iterators, counters and flags

// SPI1 Receive Buffer
int8_t SPI_Rx_Buffer[SPI_Rx_Buffer_Size];

// SPI1 Receive Buffer Iterator
uint8_t SPI_Rx_Buffer_Iter;

// SPI1 Transmit Buffer
int8_t SPI_Tx_Buffer[SPI_Tx_Buffer_Size];

// SPI1 Transmit Buffer Iterator
uint8_t SPI_Tx_Buffer_Iter;

// Flag used for receiving an interrupt request from the LoRa board
uint8_t Irq_Flag;

/* General purpose function declarations */

// function called in case of failure while operating with a peripheral
static void Error_Handler(void);

// function used for checking the Busy bit of the LoRa board
static void WaitForBusy(void);

// function used for waiting for the Interrupt pin of the LoRa board to go high
static void WaitForInterrupt(uint8_t bypass_LoRa_Tx_Message_Flag);

// function used for generating delay between commands
static void CommandDelay(uint16_t nr_of_iterations);

// function used for waiting for a specific interrupt
static uint8_t CheckInterrupt(SPI_HandleTypeDef * hspi, int8_t Irq_Mask, uint16_t nr_of_trials);

// function used for checking for LoRa transmission check
static uint8_t checkForLoRaTx(uint8_t bypass_LoRa_Tx_Message_Flag);

/* Functions */

//callback that gets called after the SPI transmit transaction is over
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
}

//callback that gets called when the SPI receive transaction is over
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
}

// function used for receiving 1 data byte from LoRa board through SPI in interrupt mode
void ReadingByteFromLoRa(SPI_HandleTypeDef *hspi)
{
	// receive 1 byte of data through SPI in interrupt mode
	if(HAL_SPI_Receive_IT(hspi, SPI_Rx_Buffer + SPI_Rx_Buffer_Iter ++, 0x01) != HAL_OK)
    {
	   Error_Handler();
    }


}

// function used for sending 1 data byte to LoRa board through SPI in interrupt mode
void WritingMessageToLoRa(SPI_HandleTypeDef *hspi)
{
	// receive 1 byte of data through SPI in interrupt mode
	if(HAL_SPI_Transmit(hspi, SPI_Tx_Buffer, SPI_Tx_Buffer_Iter, HAL_MAX_DELAY) != HAL_OK)
    {
	   Error_Handler();
    }

	// reset the SPI_Tx_Buffer_Iter
	SPI_Tx_Buffer_Iter = 0;
}



// function used for sending 1 byte of data through LoRa
uint8_t LoRa_Data_Byte_Tx(int8_t data, const uint8_t bypass_LoRa_Tx_Message_Flag)
{
	// bypass represents a flag used for stopping continuous receive in transmission mode
	if( bypass_LoRa_Tx_Message_Flag || ( (!(bypass_LoRa_Tx_Message_Flag || LoRa_Tx_Message_Flag))) )
	{
		// 0x80 - OpCode for standby
		// 0x00 - standby configuration
		int8_t SetStandby[2] = {0x80, 0x00};

		// 0x8A - OpCode for setting packet type
		// 0x01 - LoRa mode
		int8_t SetPacketType[2] = {0x8A, 0x01};

		// 0x86 - OpCode for setting RF Frequency
		// 0x36 0x48 0x00 0x00 - 32 bit value representing the frequency, in this case 868.5 MHz
		int8_t SetRfFrequency[5] = {0x86, 0x36, 0x48, 0x00, 0x00};

		// 0x8E - OpCode for setting TxParams
		// 0x00 - power of 0 dBm
		// 0x04 - RampTime of 200 us
		int8_t SetTxParams[3] = {0x8E, 0x00, 0x04};

		// 0x8F - OpCode for setting the buffer base address
		// 0x00 - base address for TxBuffer
		// 0x00 - base address for RxBuffer
		int8_t SetBufferBaseAddress[3] = {0x8F, 0x00, 0x00};

		// 0x0E - OpCode for setting the payload
		// 0x00 - offset of the data to be transmitted
		// data - byte to transmit
		int8_t SetPayload[3] = {0x0E, 0x00, data};

		// 0x8B - OpCode for setting modulation parameters
		// 0x07 - spreading factor ( in this case SP7 )
		// 0x06 - bandwidth ( in this case 500 kHz )
		// 0x01 - coding rate ( in this case 4/5 )
		// 0x00 - low data rate optimisation ( in this case off )
		int8_t SetModulationParameters[5] = {0x8B, 0x07, 0x06, 0x01, 0x00};

		// 0x8C - OpCode for setting the packet parameters
		// 0x00 - the first 8 bits of the preamble length
		// 0x10 - the last 8 bits of the preamble length
		// 0x00 - explicit header
		// 0x01 - payload length ( 1 byte )
		// 0x00 - CRC ( disabled )
		// 0x00 - IQ ( standard IQ setup )
		int8_t SetPacketParameters[7] = {0x8C, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00};

		// 0x08 - OpCode for Setting the DIO IRQ Mask
		// 0x01 0xFF - allows all the interrupts to get triggered
		// 0x01 0xFF - allows DIO1 to display all the interrupts
		// 0x00 0x00 - blocking all Interrupts from DIO2
		// 0x00 0x00 - blocking all Interrupts from DIO3
		int8_t SetDioIrqParams[9] = {0x08, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x00};

		// 0x83 - OpCode for setting the chip into TxMode
		// 0x00 - first 8 bits of the TxMode
		// 0x00 - middle 8 bits of the TxMode
		// 0x00 - last 8 bits of the TxMode ( no timeout )
		int8_t SetTxMode[4] = {0x83, 0x00, 0x00, 0x00};

		// 0x02 - OpCode for clearing the IrqStatus
		// 0x03 0xFF - setting all the bits in the interrupt flag register to clear all interrupts
		int8_t ClearIrqTx[3] = {0x02, 0x03, 0xFF};

		// IRQ mask used for checking if a receive action has been performed successfully
		int8_t IrqTxDoneMask = 0x01;

		// maximum number of trials to get the the desired IRQ status message
		uint8_t nr_of_trials = 20;

		SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetPacketType, 0x02, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetRfFrequency, 0x05, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetTxParams, 0x03, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetBufferBaseAddress, 0x03, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetPayload, 0x03, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetModulationParameters, 0x05, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetPacketParameters, 0x07, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetDioIrqParams, 0x09, 0x00, NULL, 0x00);

		WaitForBusy();

		if(!checkForLoRaTx(bypass_LoRa_Tx_Message_Flag))
		{
		   return 0;
		}

		SendAndReceiveCommand(&hspi1, SetTxMode, 0x04, 0x00, NULL, 0x00);

		WaitForBusy();

		WaitForInterrupt(bypass_LoRa_Tx_Message_Flag);

		if(!checkForLoRaTx(bypass_LoRa_Tx_Message_Flag))
		{
		   return 0;
		}

		if( !CheckInterrupt(&hspi1, IrqTxDoneMask, nr_of_trials))
	    {
		   SendAndReceiveCommand(&hspi1, ClearIrqTx, 0x03, 0x00, NULL, 0x00);

		   WaitForBusy();

	       SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

	       WaitForBusy();

	       return 0;
	    }

		SendAndReceiveCommand(&hspi1, ClearIrqTx, 0x03, 0x00, NULL, 0x00);

		WaitForBusy();

		SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

		WaitForBusy();

		return 1;
	}

	else
	{
		return 0;
	}
}

// function used for receiving 1 byte of data through LoRa
uint8_t LoRa_Data_Byte_Rx(int8_t * data, const uint8_t bypass_LoRa_Tx_Message_Flag)
{
   // bypass represents a flag used for stopping continuous receive in transmission mode
   if(bypass_LoRa_Tx_Message_Flag || (!(bypass_LoRa_Tx_Message_Flag || LoRa_Tx_Message_Flag)))
   {
	   // 0x80 - OpCode for standby
	   // 0x00 - standby configuration mode RC
	   int8_t SetStandby[2] = {0x80, 0x00};

	   // 0x8A - OpCode for setting packet type
	   // 0x01 - LoRa mode
	   int8_t SetPacketType[2] = {0x8A, 0x01};

	   // 0x86 - OpCode for setting RF Frequency
	   // 0x36 0x48 0x00 0x00 - 32 bit value representing the frequency, in this case 868.5 MHz
	   int8_t SetRfFrequency[5] = {0x86, 0x36, 0x48, 0x00, 0x00};

	   // 0x8F - OpCode for setting the buffer base address
	   // 0x00 - base address for TxBuffer
	   // 0x00 - base address for RxBuffer
	   int8_t SetBufferBaseAddress[3] = {0x8F, 0x00, 0x00};

	   // 0x8B - OpCode for setting modulation parameters
	   // 0x07 - spreading factor ( in this case SP7 )
	   // 0x06 - bandwidth ( in this case 500 kHz )
	   // 0x01 - coding rate ( in this case 4/5 )
	   // 0x00 - low data rate optimisation ( in this case off )
	   int8_t SetModulationParameters[5] = {0x8B, 0x07, 0x06, 0x01, 0x00};

	   // 0x8C - OpCode for setting the packet parameters
	   // 0x00 - the first 8 bits of the preamble length
	   // 0x10 - the last 8 bits of the preamble length
	   // 0x00 - explicit header
	   // 0x01 - payload length ( 1 byte )
	   // 0x00 - CRC ( disabled )
	   // 0x00 - IQ ( standard IQ setup )
	   int8_t SetPacketParameters[7] = {0x8C, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00};

	   // 0x08 - OpCode for Setting the DIO IRQ Mask
	   // 0x03 0xFF - allows all the interrupts to get triggered
	   // 0x03 0xFF - allows DIO1 to display all the interrupts
	   // 0x00 0x00 - blocking all Interrupts from DIO2
	   // 0x00 0x00 - blocking all Interrupts from DIO3
	   int8_t SetDioIrqParams[9] = {0x08, 0x3F, 0xFF, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x00};

	   // 0x82 - OpCode for setting the chip into RxMode
	   // 0x00 - first 8 bits of the RxMode
	   // 0x00 - middle 8 bits of the RxMode
	   // 0x00 - last 8 bits of the RxMode ( continuous mode )
	   int8_t SetRxMode[4] = {0x82, 0x00, 0x00, 0x00};

	   // if bypass_LoRa_Tx_Message_Flag is activated set a timeout of 5 seconds for
	   // the receive action performed during data transmission
	   if(bypass_LoRa_Tx_Message_Flag)
	   {
		  SetRxMode[1] = 0x04;
		  SetRxMode[2] = 0xE2;
		  SetRxMode[3] = 0x00;
	   }

       // 0x02 - OpCode for clearing the IRQ status ( to allow the receiving more data )
       // 0x03 0xFF - setting all the bits in the interrupt flag register to clear all interrupts
       int8_t ClearIrqRx[3] = {0x02, 0x03, 0xFF};

       // 0x1E - OpCode for ReadReceivedPayload
       // 0x00 - starting address ( from RxBuffer )
       // 0x01 - number of bytes to read from RxBuffer
       int8_t ReadReceivedPayload[3] = {0x1E, 0x00, 0x01};

       // IRQ mask used for checking if a receive action has been performed successfully
       int8_t IrqRxDoneMask = 0x02;

       // maximum number of trials to get the the desired IRQ status message
       uint16_t nr_of_trials = 0xFFFF;

       SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetPacketType, 0x02, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetRfFrequency, 0x05, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetBufferBaseAddress, 0x03, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetModulationParameters, 0x05, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetPacketParameters, 0x07, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetDioIrqParams, 0x09, 0x00, NULL, 0x00);

       WaitForBusy();

       if(!checkForLoRaTx(bypass_LoRa_Tx_Message_Flag))
       {
          return 0;
       }

       SendAndReceiveCommand(&hspi1, SetRxMode, 0x04, 0x00, NULL, 0x00);

       WaitForBusy();

       WaitForInterrupt(bypass_LoRa_Tx_Message_Flag);

       if(!checkForLoRaTx(bypass_LoRa_Tx_Message_Flag))
       {
          return 0;
       }

       if( !CheckInterrupt(&hspi1, IrqRxDoneMask, nr_of_trials))
       {
           SendAndReceiveCommand(&hspi1, ClearIrqRx, 0x03, 0x00, NULL, 0x00);

           WaitForBusy();

    	   SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

    	   WaitForBusy();

    	   return 0;
       }

       CommandDelay(60000);

       SendAndReceiveCommand(&hspi1, ReadReceivedPayload, 0x03, 0x01, data, 0x01);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, ClearIrqRx, 0x03, 0x00, NULL, 0x00);

       WaitForBusy();

       SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

       WaitForBusy();

       return 1;
	}

    else
    {
	   return 0;
    }
}

// function used for sending a command through SPI to the LoRa board
void SendAndReceiveCommand(SPI_HandleTypeDef * hspi, int8_t * command, uint8_t command_length, uint8_t has_response, int8_t * response, uint8_t response_length)
{
   // reset the NSS pin
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

   if(HAL_SPI_Transmit(hspi, command, command_length, HAL_MAX_DELAY) != HAL_OK)
   {
      Error_Handler();
   }

   if(has_response)
   {
	   if(HAL_SPI_Receive(hspi, response, response_length, HAL_MAX_DELAY) != HAL_OK)
	   {
	      Error_Handler();
	   }
   }

   // set the NSS pin
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
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

// function used for generating delay between commands
static void CommandDelay(uint16_t nr_of_iterations)
{
	for( uint16_t i = 0; i < nr_of_iterations; i++)
	{
	// inline assembly because why not
    __NOP();
	}
}

// function used for checking for LoRa transmission check
static uint8_t checkForLoRaTx(uint8_t bypass_LoRa_Tx_Message_Flag)
{
    if(!( bypass_LoRa_Tx_Message_Flag || ( (!(bypass_LoRa_Tx_Message_Flag || LoRa_Tx_Message_Flag)))))
    {
       int8_t ClearIrq[3] = {0x02, 0x03, 0xFF};

       SendAndReceiveCommand(&hspi1, ClearIrq, 0x04, 0x00, NULL, 0x00);

       WaitForBusy();

       int8_t SetStandby[2] = {0x80, 0x00};

       SendAndReceiveCommand(&hspi1, SetStandby, 0x02, 0x00, NULL, 0x00);

       WaitForBusy();

       return 0;
    }

    else
    {
    	return 1;
    }
}

// function used for waiting for the Busy pin of the LoRa board to go low
static void WaitForBusy(void)
{
	// keep waiting until the Busy Pin goes low
	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3));
}

// function used for waiting for the Interrupt pin of the LoRa board to go high
static void WaitForInterrupt(uint8_t bypass_LoRa_Tx_Message_Flag)
{
	// keep waiting until the Interrupt Pin goes low
	while( ((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))) && ( bypass_LoRa_Tx_Message_Flag || ( (!(bypass_LoRa_Tx_Message_Flag || LoRa_Tx_Message_Flag))) ));
}

// function used for waiting for a specific interrupt
static uint8_t CheckInterrupt(SPI_HandleTypeDef * hspi, int8_t Irq_Mask, uint16_t nr_of_trials)
{
	// 0x12 - OpCode for GetIRQStatus
    int8_t GetIrqStatus[1] = { 0x12 };
	// buffer used for storing the received IRQ status
	int8_t IrqStatus[3];

	SendAndReceiveCommand(&hspi1, GetIrqStatus, 0x01, 0x01, IrqStatus, 0x03);

	WaitForBusy();

	while( (!( IrqStatus[2] & Irq_Mask )) && ( 0 < nr_of_trials ) )
	{
	   SendAndReceiveCommand(&hspi1, GetIrqStatus, 0x01, 0x01, IrqStatus, 0x03);

	   WaitForBusy();

	   nr_of_trials --;

	}

	if( nr_of_trials == 0)
    {
	   return 0;
    }

	return 1;
}
