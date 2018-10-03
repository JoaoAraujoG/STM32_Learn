#include "Serial.h"
#include "Fifo.h"

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
uint16_t adc_value = 0;
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
	HAL_UART_Receive_IT(huart, &Rx_buffer[Rx_start_index & 127], 1);
	
	copy_to_Txbuffer(">>"); // prompt	
	HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
}



void copy_from_Rxbuffer(char* dest)
{
	while(Rx_start_index != Rx_end_index)
	{
		*dest = (char) Rx_buffer[Rx_start_index & 127];	
		dest++;
		Rx_start_index++;
	}	
}
	
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		Tx_buffer[Tx_end_index++ & 127] = Rx_buffer[Rx_end_index];
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
		
		if(Rx_buffer[Rx_end_index & 127] == '\r')
		{
			received_data = 1;
		}
		
		if(Rx_buffer[Rx_end_index & 127] == 8) //backspace
		{
			Rx_end_index --;
		}
		else
		{
			Rx_end_index++;
		}
		
		HAL_UART_Receive_IT(huart, &Rx_buffer[Rx_end_index & 127], 1);
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
	if(hadc->Instance == ADC1)
	{
		adc_value = HAL_ADC_GetValue(hadc);
		adc1_flag = 1;
	}
}

void en_gpio_clk(uint32_t GPIOX)
{
		switch(GPIOX)
		{
			case (uint32_t) GPIOA:
				__HAL_RCC_GPIOA_CLK_ENABLE();
				break;
			case (uint32_t) GPIOB:
				__HAL_RCC_GPIOB_CLK_ENABLE();
				break;
			case (uint32_t) GPIOC:
				__HAL_RCC_GPIOC_CLK_ENABLE();
				break;
			case (uint32_t) GPIOD:
				__HAL_RCC_GPIOD_CLK_ENABLE();
				break;
			case (uint32_t) GPIOE:
				__HAL_RCC_GPIOE_CLK_ENABLE();
				break;
			case (uint32_t) GPIOF:
				__HAL_RCC_GPIOF_CLK_ENABLE();
				break;
			case (uint32_t) GPIOG:
				__HAL_RCC_GPIOG_CLK_ENABLE();
				break;
			case (uint32_t) GPIOH:
				__HAL_RCC_GPIOH_CLK_ENABLE();
				break;
		}
}

int parser(char cmd_toks[8][16])
{
	char aux_str[128];
	char* token;
	int token_index = 0;
	copy_from_Rxbuffer(aux_str);
	
	//uint32_t SRAM_memory_base = 0x20000000; // to 0x3FFF FFFF
	token = strtok(aux_str, " ");
	
	strcpy(cmd_toks[token_index++], token);
	
	while(token != NULL)
	{
		token = strtok(NULL, " .");
		strcpy(cmd_toks[token_index++], token);
	}
	
	return token_index - 1;
	
	
}

void Rx_processor(UART_HandleTypeDef* huart, ADC_HandleTypeDef* hadc)
{

	
	if(received_data)
	{
		char cmd_toks[8][16];
		
		received_data = 0;
		parser(cmd_toks);
		
		copy_to_Txbuffer(">>"); // prompt
		
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
	}
	
	if(adc1_flag == 1)
	{
		char temp_str[128];
		
		adc1_flag = 0;
		temp = (((((((float)adc_value / 4095) * 3300) - 760) / 2.5) + 25));
		
		snprintf(temp_str, sizeof(temp_str), "The temperature is %f%cC\n\r", temp, 0xBA);
		copy_to_Txbuffer(temp_str);
		
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
		
	}
}


