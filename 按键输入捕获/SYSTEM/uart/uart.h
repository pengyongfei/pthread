#ifndef UART_H_
#define UART_H_
#include "stm32f4xx.h"
#include <stdio.h>
#define BUFF_SIZE 1024
typedef struct receivebuff
{
	uint8_t flag_t;
	uint8_t flag_w;
	uint16_t buff[BUFF_SIZE];
	uint16_t number;
	uint16_t len;
}receive_t;

void uart_init(uint32_t bps);
#endif

