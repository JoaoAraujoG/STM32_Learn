
#include "stm32f7xx_hal.h"

#ifndef SERIAL_H
#define SERIAL_H

extern uint8_t Rx_buffer[128];
extern uint8_t Tx_buffer[128];

extern volatile uint8_t Rx_end_index;
extern volatile uint8_t Rx_start_index;
extern volatile uint8_t Tx_end_index;
extern volatile uint8_t Tx_start_index;

//flags
extern volatile uint8_t transmiting_data;
extern volatile uint8_t received_data;
extern volatile uint8_t adc1_flag;
extern uint16_t adc1_value;
extern float temp;

void serial_init(UART_HandleTypeDef* huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void copy_from_Rxbuffer(char* dest);
void copy_to_Txbuffer(const char* message);
void Rx_processor(UART_HandleTypeDef* huart, ADC_HandleTypeDef* hadc);

#endif
