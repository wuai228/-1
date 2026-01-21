#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "USART.h"
#include "ADC+DMA.h"
#include "oled.h"
#include "IC.h"
#include "pwm.h"   // 你的 TIM3_Init() 所在头文件（如果不叫pwm.h自己改一下）

// ADC DMA 全局变量在 ADC+DMA.c 里
extern volatile uint16_t adc_buf[ADC_BUF_LEN];
extern volatile uint16_t adc_half_ready;
extern volatile uint16_t adc_full_ready;

#define HALF_LEN   (ADC_BUF_LEN/2)

static uint32_t ADC_To_mV(uint32_t adc_avg)
{
    return (adc_avg * 3300U + 2047U) / 4095U;
}

static void OLED_StaticUI(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0,  "ADC:");
    OLED_ShowString(64, 0, "mV:");
    OLED_ShowString(0, 1,  "BUF:");
    OLED_ShowString(36, 1, "----");
    OLED_Update();
}

static void OLED_ClearNumField(uint8_t x, uint8_t page, uint8_t chars)
{
    for(uint8_t i=0;i<chars;i++) OLED_ShowChar(x + i*6, page, ' ');
}

int main(void)
{
    SystemInit();
    delay_init();

    USART1_Init(115200);
    printf("Boot...\r\n");

    OLED_Init();
    printf("OLED init done\r\n");

    ADC_DMA_Init();
    printf("ADC DMA start, BUF_LEN=%d\r\n", ADC_BUF_LEN);

    // 1) PWM 输出（PA6 = TIM3_CH1）
    TIM3_Init();
    printf("PWM start: TIM3_CH1 on PA6\r\n");

    // 2) 输入捕获（PA0 = TIM2_CH1）
    IC_TIM2_CH1_PA0_Init_1MHz();
    printf("IC start : TIM2_CH1 on PA0\r\n");
    printf("Wire MUST: PA6 --> PA0\r\n");

    OLED_StaticUI();

    // ===== 运行时缓存（避免DMA边写边读）=====
    static uint16_t snap[HALF_LEN];   // 锁存一半波形数据（前半段）
    uint8_t  snap_valid = 0;

    uint32_t last_avg = 0;
    uint32_t last_mv  = 0;

    // ===== IC 最新结果缓存 =====
    IC_Result_t ic_last = {0};
    uint8_t     ic_seen = 0;

    // ===== 软件节拍调度 =====
    uint32_t tick_ms = 0;
    uint32_t t_oled  = 0;
    uint32_t t_icprt = 0;
		
		
		

    while (1)
    {
        // 10ms 节拍（不需要你改 SysTick，先用 delay 做调度就行）
        delay_ms(10);
        tick_ms += 10;

        // ---------- DMA 半满：立刻锁存前半段 ----------
        if (adc_half_ready)
        {
            adc_half_ready = 0;

            // 关键：此刻 DMA 正在写后半段，前半段是稳定的，马上 memcpy 锁存
            memcpy((void*)snap, (const void*)&adc_buf[0], HALF_LEN * sizeof(uint16_t));
            snap_valid = 1;

            last_avg = ADC_CalcAvg(snap, HALF_LEN);
            last_mv  = ADC_To_mV(last_avg);
        }

        // ---------- DMA 满：这里只做清标志/可选统计 ----------
        if (adc_full_ready)
        {
            adc_full_ready = 0;
            // 如果你想用后半段也更新 avg，可仿照上面再 memcpy 一个 snap2
        }

        // ---------- 取 IC 结果：有就缓存 ----------
        {
            IC_Result_t tmp;
            if (IC_TakeResult(&tmp))
            {
                ic_last = tmp;
                ic_seen = 1;
            }
        }

        // ---------- OLED：每 100ms 刷一次（别跟DMA频率跑） ----------
        if ((tick_ms - t_oled) >= 100)
        {
            t_oled = tick_ms;

            // 数字
            OLED_ClearNumField(24, 0, 6);
            OLED_ClearNumField(88, 0, 5);
            OLED_ShowNum(24, 0, last_avg);
            OLED_ShowNum(88, 0, last_mv);

            // 波形：用锁存数据画（非常关键）
            if (snap_valid)
            {
                OLED_DrawWave_Adc128(snap, 16, 48, 1);  // 你原来的函数
            }

            OLED_Update();
        }

        // ---------- 串口输出 IC：每 200ms 打印一次 ----------
        if ((tick_ms - t_icprt) >= 200)
        {
            t_icprt = tick_ms;

            if (ic_seen)
            {
                // 不用 float 直接 printf（Keil float printf 经常坑）
                uint32_t freq = (uint32_t)(ic_last.freq_hz + 0.5f);
                uint32_t duty_x10 = (uint32_t)(ic_last.duty * 10.0f + 0.5f);

                printf("IC: Tcnt=%lu Hcnt=%lu  F=%luHz  Duty=%lu.%lu%%\r\n",
                       (unsigned long)ic_last.Tcnt,
                       (unsigned long)ic_last.Hcnt,
                       (unsigned long)freq,
                       (unsigned long)(duty_x10/10),
                       (unsigned long)(duty_x10%10));
            }
            else
            {
                printf("IC: no signal (check PA6->PA0 wire)\r\n");
            }
        }
    }
}
