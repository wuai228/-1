#include "ADC+DMA.h"

volatile uint16_t adc_buf[ADC_BUF_LEN];
volatile uint16_t adc_half_ready = 0;
volatile uint16_t adc_full_ready = 0;

static void DMA_ADC_Init(void)
{
	DMA_InitTypeDef dma;
	NVIC_InitTypeDef nvic;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Channel1);
	dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	dma.DMA_MemoryBaseAddr = (uint32_t)adc_buf;
	dma.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma.DMA_BufferSize = ADC_BUF_LEN;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_M2M = DMA_M2M_Disable;
	
	
	DMA_Init(DMA1_Channel1,&dma);
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_HT | DMA_IT_TC,ENABLE);
	
	nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&nvic);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

static void ADC1_Config_Channel1_PA1(void)
{
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1,ENABLE);
	
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	gpio.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA,&gpio);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	ADC_DeInit(ADC1);
	
	adc.ADC_Mode = ADC_Mode_Independent;
	adc.ADC_ScanConvMode = DISABLE;
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1,&adc);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
	
	ADC_DMACmd(ADC1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
}

void ADC_DMA_Init()
{
	adc_half_ready = 0;
	adc_full_ready = 0;
	
	DMA_ADC_Init();
	ADC1_Config_Channel1_PA1();
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_HT1)!= RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_HT1);
		adc_half_ready = 1;
	}
	
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		adc_full_ready = 1;
	}
}


uint32_t ADC_CalcAvg(const volatile uint16_t* p,uint16_t n)
{
	uint32_t sum = 0;
	for(uint16_t i =0;i<n;i++)sum += p[i];
	return sum/n;
	
}

float ADC_AvgToVoltage(uint32_t avg)
{
	return (avg / 4095.0f)*3.3f;
}



	
	
	
	
	