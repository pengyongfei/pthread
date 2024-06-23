#include "uart.h"
#include "stm32f4xx.h"                  // Device header


#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

void uart_init(uint32_t bps)
{
	GPIO_InitTypeDef gpio_uart;
	USART_InitTypeDef usart_init;
	NVIC_InitTypeDef nvic_init;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
//	RCC_AHB1PeriphClockLPModeCmd(RCC_AHB1Periph_GPIOA,ENABLE);
//	RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_USART1,ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//复用成串口
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//复用成串口
	
	gpio_uart.GPIO_Pin	=	GPIO_Pin_9|GPIO_Pin_10;
	gpio_uart.GPIO_Mode =	GPIO_Mode_AF;
	gpio_uart.GPIO_OType=	GPIO_OType_PP;
	gpio_uart.GPIO_PuPd	=	GPIO_PuPd_NOPULL;
	gpio_uart.GPIO_Speed=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio_uart);
	
	//USART1 初始化设置
	usart_init.USART_BaudRate = bps;//波特率设置
	usart_init.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	usart_init.USART_StopBits = USART_StopBits_1;//一个停止位
	usart_init.USART_Parity = USART_Parity_No;//无奇偶校验位
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &usart_init); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启接收中断
	nvic_init.NVIC_IRQChannel			=	USART1_IRQn	;
	nvic_init.NVIC_IRQChannelCmd	=	ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_init.NVIC_IRQChannelSubPriority				= 3;
	NVIC_Init(&nvic_init);
}

void USART1_IRQHandler(void)
{
	receive_t rece;
	rece.len = 0;
	rece.number = 0;
	//uint16_t rev = 0;
	printf("11\r\n");
	if(USART_GetFlagStatus(USART1,USART_IT_RXNE) != RESET)
	{
		//rev = USART_ReceiveData(USART1);
		rece.buff[rece.number] = USART_ReceiveData(USART1);
		rece.number++;
		rece.len++;
		USART_SendData(USART1,rece.buff[rece.number]);
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
}
void stremp(receive_t *rece)
{
	if(rece->buff[0]==0x00)
	{
		rece->flag_t =1;
		
	}
	else
	{
		rece->flag_t = 0;
	}
}

          



