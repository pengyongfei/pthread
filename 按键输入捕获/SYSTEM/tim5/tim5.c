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
	tim5_ch1.TIM_CounterMode							=		TIM_CounterMode_Up;//计数模式为向上计数
	tim5_ch1.TIM_Period										=		arr;//重装载值寄存器
	tim5_ch1.TIM_ClockDivision						=		TIM_CKD_DIV1;
	
	//tim5_ch1.TIM_RepetitionCounter				=		重复计数寄存器只有tim1和time8才有用到
	TIM_TimeBaseInit(TIM5,&tim5_ch1);
	tim5_input_ch1.TIM_Channel						=		TIM_Channel_1; 
	tim5_input_ch1.TIM_ICPolarity					=		TIM_ICPolarity_Rising;//上升沿捕获
	tim5_input_ch1.TIM_ICSelection				=		TIM_ICSelection_DirectTI;//映射到TI1上
	tim5_input_ch1.TIM_ICPrescaler				=		TIM_ICPSC_DIV1;//配置输入分频
	tim5_input_ch1.TIM_ICFilter						=		0X00;//不滤波
	TIM_ICInit(TIM5,&tim5_input_ch1);
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//开启更新中断和cc1捕获中断
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
					计数位【5~0】	【6】：第一次表示已经捕获高电平，第二次表示捕获到低电平
			*/
			if(tim5_ch1_sta&0X40)//已经捕获到了高电平
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
			if(tim5_ch1_sta&0x40)//已经捕获到了低电平
			{
				printf("tim5_ch1_sta&0x40=%#X\r\n",tim5_ch1_sta&0x40);
				printf("33\r\n");
				tim5_ch1_sta |=0x80;
				tim5_ch1_val 	= TIM_GetCapture1(TIM5);//把获取到的值给到val
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);//将级性设置成高电平触发方便下次使用
			}
			else
			{
				printf("44\r\n");
				tim5_ch1_sta		=	0;
				tim5_ch1_val		= 0;
				tim5_ch1_sta	 |=	0x40;//已经获取到了上升沿
				TIM_Cmd(TIM5,DISABLE);//关闭定时器
				TIM_SetCounter(TIM5,0);//将计数器清0
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);//设置成下降沿触发
				TIM_Cmd(TIM5,ENABLE);//打开定时器
				
			}
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update|TIM_IT_CC1);//清除中断标志位
}
