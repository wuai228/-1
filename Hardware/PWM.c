#include "stm32f10x.h"                  // Device header



void TIM3_Init()
{
	
	
	GPIO_InitTypeDef  gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_6;				 
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; 		 
	gpio.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOA, &gpio);		
	
	
	
	TIM_TimeBaseInitTypeDef tim;
	//NVIC_InitTypeDef nvic;
	TIM_OCInitTypeDef oc;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	tim.TIM_Period = 1000-1;
	tim.TIM_Prescaler = 720-1;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM1;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_Pulse = 500;
	oc.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3,&oc);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	
	
}