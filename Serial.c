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

char** parser()
{
	char aux_str[128];
	char token_arr[16][128];
	char* token;
	int token_index = 0;
	char* teste = " kill me pls";
	copy_from_Rxbuffer(aux_str);
	
	//uint32_t SRAM_memory_base = 0x20000000; // to 0x3FFF FFFF
	token = strtok(aux_str, " ");
	
	//strcpy(token_arr[token_index], "pooped");
	
	while(token != NULL)
	{
		token = strtok(NULL, " .");
		//strcpy(token_arr[token_index], token);
		token_index++;
	}
	
	/*if(strcmp(token, "MW") == 0)
	{
		uint32_t* memory_pos ;
		
		token = strtok(NULL, " ");
		memory_pos = (uint32_t*)(SRAM_memory_base | (uint32_t)strtol(token, NULL, 16));
		
		token = strtok(NULL, " ");
		*memory_pos = (uint8_t)strtol(token, NULL, 16);
		
		char mw_succ[128];
		snprintf(mw_succ, sizeof(mw_succ), "The data value [0x%02x] was written in memory position [0x%04x]\n\r", *memory_pos, (uint16_t) memory_pos);
		copy_to_Txbuffer(mw_succ);
		
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
	}
	else if(strcmp(token, "MR") == 0)
	{
		uint32_t* memory_pos ;
		
		token = strtok(NULL, " ");
		memory_pos = (uint32_t*)(SRAM_memory_base | (uint32_t)strtol(token, NULL, 16));
		
		char mr_succ[128];
		snprintf(mr_succ, sizeof(mr_succ), "The data value present in memory position [0x%04x] is [0x%02x]\n\r", (uint16_t) memory_pos, *memory_pos);
		copy_to_Txbuffer(mr_succ);
		
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
	}
	else if(strcmp(token, "MO") == 0)
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		uint32_t GPIOX; 
		uint16_t pins;
		
		token = strtok(NULL, " ");
		GPIOX = (uint32_t)strtol(token, NULL, 16);
		
		token = strtok(NULL, " ");
		pins = (uint32_t)strtol(token, NULL, 16);
		
		en_gpio_clk(GPIOX);
		
		GPIO_InitStruct.Pin = pins;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init((GPIO_TypeDef*)GPIOX, &GPIO_InitStruct);
	}
	else if(strcmp(token, "WD") == 0)
	{
		GPIO_TypeDef* GPIOX; 
		uint16_t pins;
		
		token = strtok(NULL, " ");
		GPIOX = (GPIO_TypeDef*)strtol(token, NULL, 16);
		
		token = strtok(NULL, " ");
		pins = (uint16_t)strtol(token, NULL, 16);
		
		GPIOX->ODR = pins;
		
	}
	else if(strcmp(token, "RA") == 0)
	{
		HAL_ADC_Start_IT(hadc);
	}
	else
	{
		copy_to_Txbuffer("Unknown command revieved\n\r");
		
		HAL_UART_Transmit_IT(huart, &Tx_buffer[Tx_start_index & 127], 1);
	}
	*/
}

void Rx_processor(UART_HandleTypeDef* huart, ADC_HandleTypeDef* hadc)
{
	
	if(received_data)
	{
		received_data = 0;
		parser(huart, hadc);
		
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


