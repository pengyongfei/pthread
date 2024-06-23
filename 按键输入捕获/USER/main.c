#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"	
//#include "uart.h"
#include "tim5.h"
int main(void)
{
//	uint8_t 	tim5_ch1_sta;
//	uint32_t	tim5_ch1_val;
	long long temp = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//采用中断分组2
	//uart_init(115200);
	delay_init(168);
	uart_init(115200);
	tim5_ch1_init(0XFFFFFFFF,84-1);
//	printf("你好世界\r\n");
//	printf("%d\r\n",TIM5->SR);
	while(1)
	{
		delay_ms(10);
		if(tim5_ch1_sta&0x80)
		{
			//printf("%#X\r\n",tim5_ch1_sta);
			temp 	= tim5_ch1_sta&0x3f;
			temp *=	0XFFFFFFFF;
			temp +=	tim5_ch1_val;
			printf("hight:%lldus\r\n",temp);
			tim5_ch1_sta = 0;
		}
	}
}





