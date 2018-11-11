#include "Serial.h"

#include <stdlib.h>
#include <string.h>

uint8_t Rx_buffer[128] = {0};
uint8_t Tx_buffer[128] = {0};

volatile uint8_t Rx_end_index = 0;
volatile uint8_t Rx_start_index = 0;
volatile uint8_t Tx_end_index = 0;
volatile uint8_t Tx_start_index = 0;

//flags
volatile uint8_t transmiting_data = 0;
volatile uint8_t received_data = 0;
volatile uint8_t adc1_flag = 0;
uint16_t adc1_value = 0;
float temp = 0;

void copy_to_Txbuffer(const char* message)
{
	while(*message != '\0')
	{
		Tx_buffer[Tx_end_index++ & 127] = (uint8_t) *message;	
		message++;
	}	
}

void serial_init(UART_HandleTypeDef* huart)
{
	HAL_UART_Receive_IT(huart, &Rx_buffer[Rx_end_index & 127], 1);
	
	copy_to_Txbuffer(">>"); // prompt	
	HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
}

void copy_from_Rxbuffer(char* dest)
{
	while(Rx_start_index != Rx_end_index)		// Rx_buffer is 
	{
		*dest = (char) Rx_buffer[Rx_start_index & 127];		//  Rx_buffer caracter is copied to the destination string
		dest++;																						//	Incremente destination pointer to the next free byte
		
		Rx_start_index++;		// Caracter is removed from rx_buffer after being copied
	}	
	
	*dest = '\0';		// String terminator caracter is added
}
	
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		Tx_buffer[Tx_end_index++ & 127] = Rx_buffer[Rx_end_index & 127];		//	Las bit received is copied to tx_buffer
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);		//	Last bit received is transmited (echo)
		
		received_data = Rx_buffer[Rx_end_index & 127] == '\r';		//	If the end of the input is reached
																															//	Recieved_data flag is activated	
	
		Rx_end_index -= 2*(Rx_buffer[Rx_end_index & 127] == 8);		//	If rx_buffer value is 8 (backspace) 
																															//	the latest value added is removed
		Rx_end_index += 1 ^ received_data; 												//  After all data is received, no caracter is added
			
		HAL_UART_Receive_IT(huart, &Rx_buffer[Rx_end_index & 127], 1);		//	Interuption rearmed
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		Tx_start_index++;
		
		if(!(Tx_end_index == Tx_start_index)) // not empty
		{
			HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
		}	
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	adc1_value = HAL_ADC_GetValue(hadc);
	adc1_flag = 1;
	
}




