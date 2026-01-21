#include "IC.h"

static volatile uint16_t t1 = 0,tf = 0,t2 = 0;
static volatile uint8_t step = 0;
static volatile IC_Result_t g = {0};


static uint32_t diff16(uint16_t a,uint16_t b)
{
	return(uint16_t)(b-a);
}

void IC_TIM2_CH1_PA0_Init_1MHz(void)
{
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef tim;
	TIM_ICInitTypeDef ic;
	NVIC_InitTypeDef nvic;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&gpio);
	
	tim.TIM_Prescaler = 72-1;
	tim.TIM_Period = 0xFFFF;
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&tim);
	
	ic.TIM_Channel = TIM_Channel_1;
	ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM2,&ic);
	
	TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
	
	nvic.NVIC_IRQChannel = TIM2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority  = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	TIM_Cmd(TIM2,ENABLE);
}

uint8_t IC_TakeResult(IC_Result_t* out)
{
	if(!g.ready)return 0;
	*out = g;
	g.ready = 0;
	return 1;
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
		
		uint16_t cap = TIM_GetCapture1(TIM2);
		
		if(step ==0)
		{
			t1 = cap;
			step = 1;
			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);
		}
		else if(step == 1)
		{
			tf =cap;
			step = 2;
			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising);
		}
		else{
			t2 = cap;
			step = 0;
			uint32_t Tcnt = diff16(t1,t2);
			uint32_t Hcnt = diff16(t1,tf);
			
			if(Tcnt != 0&&Hcnt <=Tcnt)
			{
				g.Tcnt = Tcnt;
				g.Hcnt = Hcnt;
				g.freq_hz = 1000000.0f/(float)Tcnt;
				g.duty = (float)Hcnt * 100.0f / (float)Tcnt;
				g.ready = 1;
			}
		}
	}
}
