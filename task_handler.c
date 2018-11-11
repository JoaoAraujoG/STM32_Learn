#include "serial.h"
#include "fifo.h"
#include "i2c-lcd.h"
#include "hum_tem_sens.h"

#include <string.h>
#include <stdlib.h>

/**** GLOBAL VARIABLES ****/

// Task list variables 
void* task_list[16]; 	// Circular buffer of all the pointers to task functions
int tl_head = 0;			// Index for the head of the task list
int tl_tail = 0;			// Index for the tail of the task list


uint32_t adc1_ch = 0; // ADC1 chanel address

/**************************/

/**** TASK LIST MANAGEMENT FUNCTIONS ****/

void enqueue(void* func_ptr) // Enqueues a function pointer to the task list
{
	tl_head++; 													// Head index is incremented before adding a task
	task_list[tl_head & 15] = func_ptr; // Task is added into the task list
}

void* unqueue() // Removes and returns the oldest task added to the task list
{
	void* func_ptr = NULL; 							// Auxiliar variable that will hold the return value
	tl_tail++;													// Head index is incremented after removing a task
	func_ptr = task_list[tl_tail & 15]; // Oldest task is poped from the task list
	return func_ptr;										// The removed task is returned
}

/****************************************/

int parser(char cmd_toks[8][16])
{
	
	char aux_str[32];
	char* token;
	int token_index = 0;
	
	copy_from_Rxbuffer(aux_str);
	
	token = strtok(aux_str, " ");
	
	strcpy(cmd_toks[token_index++], token);
	
	while(token != NULL)
	{
		token = strtok(NULL, " .");
		strcpy(cmd_toks[token_index++], token);
	}
	
	return token_index - 1;

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


void memory_write(char cmd_toks[8][16])
{
		uint32_t* memory_pos ;
		uint32_t SRAM_memory_base = 0x20000000; // to 0x3FFF FFFF
	
		memory_pos = (uint32_t*)(SRAM_memory_base | (uint32_t)strtol(cmd_toks[1], NULL, 16));
		*memory_pos = (uint8_t)strtol(cmd_toks[2], NULL, 16);
		
		char mw_succ[128];
		snprintf(mw_succ, sizeof(mw_succ), "The data value [0x%02x] was written in memory position [0x%04x]\n\r", *memory_pos, (uint16_t) memory_pos);
		copy_to_Txbuffer(mw_succ);
}

void memory_read(char cmd_toks[8][16])
{
		uint32_t* memory_pos ;
		uint32_t SRAM_memory_base = 0x20000000; // to 0x3FFF FFFF
		
		memory_pos = (uint32_t*)(SRAM_memory_base | (uint32_t)strtol(cmd_toks[1], NULL, 16));
		
		char mr_succ[128];
		snprintf(mr_succ, sizeof(mr_succ), "The data value present in memory position [0x%04x] is [0x%02x]\n\r", (uint16_t) memory_pos, *memory_pos);
		copy_to_Txbuffer(mr_succ);
		
}

void make_output(char cmd_toks[8][16])
{
	GPIO_InitTypeDef GPIO_InitStruct;
		
		uint32_t GPIOX; 
		uint16_t pins;
		
		GPIOX = (uint32_t)strtol(cmd_toks[1], NULL, 16);
		
		pins = (uint32_t)strtol(cmd_toks[2], NULL, 16);
		
		en_gpio_clk(GPIOX);
		
		GPIO_InitStruct.Pin = pins;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init((GPIO_TypeDef*)GPIOX, &GPIO_InitStruct);
}

void write_digital(char cmd_toks[8][16])
{
	
		GPIO_TypeDef* GPIOX; 
		uint16_t pins;
		
		GPIOX = (GPIO_TypeDef*)strtol(cmd_toks[1], NULL, 16);
		
		pins = (uint16_t)strtol(cmd_toks[2], NULL, 16);
		
		GPIOX->ODR = pins;
}

void read_adc()
{
	
if(adc1_flag == 1)
	{
		adc1_flag = 0;
		
		if(adc1_ch == ADC_CHANNEL_TEMPSENSOR)
		{
		char temp_str[64];
			
		
		temp = (((((((float)adc1_value / 255) * 3300) - 760) / 2.5) + 25));
		
  	snprintf(temp_str, sizeof(temp_str), "The temperature is %f%cC\n\r", temp, 0xBA);
		copy_to_Txbuffer(temp_str);
		
		}
		
		if(adc1_ch == ADC_CHANNEL_0)
		{
		
		char ch0_value[64];
		float voltage;
			
		voltage = (float) adc1_value *( 3.3 / 255 );
		snprintf(ch0_value, sizeof(ch0_value), "ADC1 ch0 value is %fV\n\r", voltage);
		copy_to_Txbuffer(ch0_value);
		
		}
	}
}

void read_analog(char cmd_toks[8][16])
{
	ADC_ChannelConfTypeDef sConfig;
	void (*func_ptr)() = &read_adc;
	
	enqueue(func_ptr);
	
	ADC_HandleTypeDef* adc_addr = (ADC_HandleTypeDef*)strtol(cmd_toks[1], NULL, 16);
	adc1_ch = (uint32_t)strtol(cmd_toks[2], NULL, 16);
	
	sConfig.Channel = adc1_ch;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	
  if (HAL_ADC_ConfigChannel(adc_addr, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	
	HAL_ADC_Start_IT(adc_addr);
	
}

void temp_hum_sens_handler(char cmd_toks[8][16])
{
	char output_str[64];
	
	if(strcmp(cmd_toks[1], "TEMP") == 0)		//	If the user wants to read the temperature
	{
		snprintf(output_str, sizeof(output_str), "Current temperature is %f%cC\n\r", get_temperature(),0xBA);
	}
	else if(strcmp(cmd_toks[1], "HUM") == 0)		// If the user wants to read the relative humidity
	{
		snprintf(output_str, sizeof(output_str), "Current relative humidity is %f%c\n\r", get_humidity(),0x25);
	}
	else
	{
		snprintf(output_str, sizeof(output_str), "Sintax Error\n\r");
	}
	
	copy_to_Txbuffer(output_str);		// Send measured value through UART
}

void uart_cmd_process(char cmd_toks[8][16])
{
	
	if(strcmp(cmd_toks[0], "MW") == 0)
	{
		memory_write(cmd_toks);
	}
	else if(strcmp(cmd_toks[0], "MR") == 0)
	{
		memory_read(cmd_toks);
	}
	else if(strcmp(cmd_toks[0], "MO") == 0)
	{
		make_output(cmd_toks);
	}
	else if(strcmp(cmd_toks[0], "WD") == 0)
	{
		write_digital(cmd_toks);
	}
	else if(strcmp(cmd_toks[0], "RA") == 0)
	{
		read_analog(cmd_toks);
	}
	else if(strcmp(cmd_toks[0], "WLCD") == 0)
	{
		lcd_clear();										// Clear lcd screen
		lcd_send_string(cmd_toks[1]);		// Write second token to lcd
	}
	else if(strcmp(cmd_toks[0], "THS") == 0)		// Temperature and humidity sensor command 
	{
		temp_hum_sens_handler(cmd_toks);		
	}
	else
	{
		copy_to_Txbuffer("Unknown command recieved\n\r");
	}
	
}



int perform_next_task() // Takes care of performing the next task on the list
{
	void (*func_ptr)() = NULL; // Declaration of a pointer to a function
	
	if(tl_head == tl_tail)
		return 1;
	
	func_ptr = unqueue();			 // The function poiter is assigned with the next task on the list
	(*func_ptr)();						 // The task is performed
	
	return 0; // Returns 1 if there are no more tasks to perform, 0 otherwise
}

