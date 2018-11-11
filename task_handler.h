#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

extern uint32_t adc1_ch;

void uart_cmd_process(char cmd_toks[8][16]);
int parser(char cmd_toks[8][16]); 							// breaks Rx message into tokens and returns the number of tokens
int perform_next_task();												// Takes care of performing the next task on the list

#endif
