#ifndef __IC_H
#define __IC_H

#include "stm32f10x.h"
#include <stdint.h>

typedef struct {
    uint32_t Tcnt;
    uint32_t Hcnt;
    float    freq_hz;
    float    duty;
    uint8_t  ready;
} IC_Result_t;

void IC_TIM2_CH1_PA0_Init_1MHz(void);
uint8_t IC_TakeResult(IC_Result_t* out);   // 有新数据返回1

#endif
