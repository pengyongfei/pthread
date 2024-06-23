#ifndef TIM5_H_
#define TIM5_H_
#include "stm32f4xx.h"
#include "usart.h"
void tim5_ch1_init(uint32_t arr,uint16_t psc);
extern uint8_t 	tim5_ch1_sta;
extern uint32_t	tim5_ch1_val;

#endif

