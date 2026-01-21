#ifndef __DMA_H
#define __DMA_H

#include "stm32f10x.h"                  // Device header


#define ADC_BUF_LEN 256

extern volatile uint16_t adc_buf[ADC_BUF_LEN];
extern volatile uint16_t adc_half_ready;
extern volatile uint16_t adc_full_ready;

void ADC_DMA_Init(void);
void DMA_ADC_Init(void);

uint32_t ADC_CalcAvg(const volatile uint16_t* p,uint16_t n);
float ADC_AvgToVotage(uint32_t avg);

#endif
