#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"
#include <stdint.h>

#define OLED_W 128
#define OLED_H 64

// 默认地址（7bit）。实际会在 OLED_Init 里自动探测 0x3C/0x3D
#ifndef OLED_ADDR_7BIT_DEFAULT
#define OLED_ADDR_7BIT_DEFAULT 0x3C
#endif

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);

void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t color);

void OLED_ShowChar(uint8_t x, uint8_t page, char c);
void OLED_ShowString(uint8_t x, uint8_t page, const char *s);
void OLED_ShowNum(uint8_t x, uint8_t page, uint32_t num);
void OLED_ShowSignedNum(uint8_t x, uint8_t page, int32_t num);

uint8_t OLED_MapAdcToY(uint16_t adc, uint8_t y_top, uint8_t height);
void OLED_DrawWave_Adc128(const volatile uint16_t *p128,
                          uint8_t y_top, uint8_t height,
                          uint8_t clear_before);

#endif
