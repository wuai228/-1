#include "delay.h"

static uint32_t fac_us = 0;   // 1us 对应的 SysTick 计数值（用HCLK时）
static uint32_t fac_ms = 0;   // 1ms 对应的 SysTick 计数值

void delay_init(void)
{
    // 选择 SysTick 时钟源为 HCLK（= SystemCoreClock）
    // SPL里也可以用 SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    // 计算 1us / 1ms 对应的计数值
    fac_us = SystemCoreClock / 1000000U;
    fac_ms = fac_us * 1000U;

    // 先关闭 SysTick
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief  微秒延时（阻塞）
 * @note   SysTick LOAD 是 24bit，单次最大计数为 0xFFFFFF
 */
void delay_us(uint32_t us)
{
    if (us == 0) return;

    uint32_t max_us_once = 0xFFFFFFU / fac_us;   // 单次最多能延时多少 us

    while (us)
    {
        uint32_t cur = (us > max_us_once) ? max_us_once : us;
        us -= cur;

        SysTick->LOAD = cur * fac_us - 1U;   // 装载计数值
        SysTick->VAL  = 0U;                  // 清零当前计数
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        // 等待 COUNTFLAG 置位（计数到 0）
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { }

        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }
}

/**
 * @brief 毫秒延时（阻塞）
 */
void delay_ms(uint32_t ms)
{
    if (ms == 0) return;

    // 也可以直接循环 delay_us(1000)，这里用 fac_ms 更高效
    // 但同样受 24bit 限制，所以分段
    uint32_t max_ms_once = 0xFFFFFFU / fac_ms;

    while (ms)
    {
        uint32_t cur = (ms > max_ms_once) ? max_ms_once : ms;
        ms -= cur;

        SysTick->LOAD = cur * fac_ms - 1U;
        SysTick->VAL  = 0U;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { }

        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }
}
