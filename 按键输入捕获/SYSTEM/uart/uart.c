#include "uart.h"
#include "stm32f4xx.h"                  // Device header


#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

void uart_init(uint32_t bps)
{
	GPIO_InitTypeDef gpio_uart;
	USART_InitTypeDef usart_init;
	NVIC_InitTypeDef nvic_init;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
//	RCC_AHB1PeriphClockLPModeCmd(RCC_AHB1Periph_GPIOA,ENABLE);
//	RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_USART1,ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//���óɴ���
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//���óɴ���
	
	gpio_uart.GPIO_Pin	=	GPIO_Pin_9|GPIO_Pin_10;
	gpio_uart.GPIO_Mode =	GPIO_Mode_AF;
	gpio_uart.GPIO_OType=	GPIO_OType_PP;
	gpio_uart.GPIO_PuPd	=	GPIO_PuPd_NOPULL;
	gpio_uart.GPIO_Speed=	GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio_uart);
	
	//USART1 ��ʼ������
	usart_init.USART_BaudRate = bps;//����������
	usart_init.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	usart_init.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	usart_init.USART_Parity = USART_Parity_No;//����żУ��λ
	usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &usart_init); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//���������ж�
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

          



