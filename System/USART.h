#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void USART1_Init(uint32_t baudrate);//串口初始化

void USART1_SendByte(uint8_t date);//发送一个字节

void USART1_SendString(const char *str);//发送一个字符串

uint8_t USART1_ReceiveByte(void);

#endif
