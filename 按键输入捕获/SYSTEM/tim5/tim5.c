#include "tim5.h"
#include "stm32f4xx.h"                  // Device header
static void tim5_gpio_init(void)
{
	GPIO_InitTypeDef gpio_ch1;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	gpio_ch1.GPIO_Pin					= 					GPIO_Pin_0;
	gpio_ch1.GPIO_Mode				=						GPIO_Mode_AF;
	gpio_ch1.GPIO_OType				=						GPIO_OType_PP;
	gpio_ch1.GPIO_PuPd				=						GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL//GPIO_PuPd_DOWN
	gpio_ch1.GPIO_Speed				=						GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&gpio_ch1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
}
static void tim5_nvic(void)
{
	NVIC_InitTypeDef		nvic_init;
	nvic_init.NVIC_IRQChannel												=		 TIM5_IRQn;//USART1_IRQn	;
	nvic_init.NVIC_IRQChannelCmd										=		 ENABLE;
	nvic_init.NVIC_IRQChannelPreemptionPriority 		=    2;
	nvic_init.NVIC_IRQChannelSubPriority						=    0;
	NVIC_Init(&nvic_init);
}
void tim5_ch1_init(uint32_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef tim5_ch1;
	TIM_ICInitTypeDef tim5_input_ch1;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	tim5_gpio_init();
	tim5_ch1.TIM_Prescaler								=		psc;
	tim5_ch1.TIM_CounterMode							=		TIM_CounterMode_Up;//����ģʽΪ���ϼ���
	tim5_ch1.TIM_Period										=		arr;//��װ��ֵ�Ĵ���
	tim5_ch1.TIM_ClockDivision						=		TIM_CKD_DIV1;
	
	//tim5_ch1.TIM_RepetitionCounter				=		�ظ������Ĵ���ֻ��tim1��time8�����õ�
	TIM_TimeBaseInit(TIM5,&tim5_ch1);
	tim5_input_ch1.TIM_Channel						=		TIM_Channel_1; 
	tim5_input_ch1.TIM_ICPolarity					=		TIM_ICPolarity_Rising;//�����ز���
	tim5_input_ch1.TIM_ICSelection				=		TIM_ICSelection_DirectTI;//ӳ�䵽TI1��
	tim5_input_ch1.TIM_ICPrescaler				=		TIM_ICPSC_DIV1;//���������Ƶ
	tim5_input_ch1.TIM_ICFilter						=		0X00;//���˲�
	TIM_ICInit(TIM5,&tim5_input_ch1);
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//���������жϺ�cc1�����ж�
	TIM_Cmd(TIM5,ENABLE);
	tim5_nvic();
}

uint8_t 	tim5_ch1_sta = 0;
uint32_t	tim5_ch1_val = 0;
void TIM5_IRQHandler(void)
{
	if((tim5_ch1_sta&0X80)==0)
	{
		if(TIM_GetITStatus(TIM5,TIM_IT_Update)!=RESET)
		{
			printf("11\r\n");
			/**
			8	4	2	1	8	4	2	1
					����λ��5~0��	��6������һ�α�ʾ�Ѿ�����ߵ�ƽ���ڶ��α�ʾ���񵽵͵�ƽ
			*/
			if(tim5_ch1_sta&0X40)//�Ѿ������˸ߵ�ƽ
			{
				if((tim5_ch1_sta&0X3f)==0x3f)
				{
					tim5_ch1_sta	|=	0x80;
					tim5_ch1_val	 =	0xffffffff;
				}
				else
					tim5_ch1_val++;
			}
		}
		if(TIM_GetITStatus(TIM5,TIM_IT_CC1)!=RESET)
		{
			printf("22\r\n");
			if(tim5_ch1_sta&0x40)//�Ѿ������˵͵�ƽ
			{
				printf("tim5_ch1_sta&0x40=%#X\r\n",tim5_ch1_sta&0x40);
				printf("33\r\n");
				tim5_ch1_sta |=0x80;
				tim5_ch1_val 	= TIM_GetCapture1(TIM5);//�ѻ�ȡ����ֵ����val
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);//���������óɸߵ�ƽ���������´�ʹ��
			}
			else
			{
				printf("44\r\n");
				tim5_ch1_sta		=	0;
				tim5_ch1_val		= 0;
				tim5_ch1_sta	 |=	0x40;//�Ѿ���ȡ����������
				TIM_Cmd(TIM5,DISABLE);//�رն�ʱ��
				TIM_SetCounter(TIM5,0);//����������0
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);//���ó��½��ش���
				TIM_Cmd(TIM5,ENABLE);//�򿪶�ʱ��
				
			}
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update|TIM_IT_CC1);//����жϱ�־λ
}
