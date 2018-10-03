#include "task_handler.h"

void set_task(char cmd_toks[8][16])
{
	int token_index = 0;
	
	if(strcmp(cmd_toks[token_index++], "MW") == 0)
	{
		uint32_t* memory_pos ;
		
		memory_pos = (uint32_t*)(SRAM_memory_base | (uint32_t)strtol(cmd_toks[token_index++], NULL, 16));
		*memory_pos = (uint8_t)strtol(cmd_toks[token_index], NULL, 16);
		
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
	
}