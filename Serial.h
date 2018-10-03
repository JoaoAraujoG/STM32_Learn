
#include "stm32f7xx_hal.h"

#ifndef SERIAL_H
#define SERIAL_H

void serial_init(UART_HandleTypeDef* huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
char** parser();
void Rx_processor(UART_HandleTypeDef* huart, ADC_HandleTypeDef* hadc);

#endif
