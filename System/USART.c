#include "stm32f10x.h"       // Device header
#include "stdio.h"

#pragma import(__use_no_semihosting)

// 必须提供这些，避免库去找半主机
struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

void _sys_exit(int x)
{
    x = x;
    while (1);
}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}



static void USART1_GPIO_Config(void)
{
	GPIO_InitTypeDef gpio;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO| RCC_APB2Periph_USART1,ENABLE);
	
	
	//TX
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
	
	//RX
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpio);
}

void USART1_Init(uint32_t baudrate)
{
	USART_InitTypeDef usart1;
	
	USART1_GPIO_Config();
	usart1.USART_BaudRate = baudrate;
	usart1.USART_WordLength = USART_WordLength_8b;
	usart1.USART_StopBits = USART_StopBits_1;
	usart1.USART_Parity = USART_Parity_No;
	usart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart1.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_Init(USART1,&usart1);
	USART_Cmd(USART1,ENABLE);
	
}

void USART1_SendByte(uint8_t data)
{
	USART_SendData(USART1,data);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}

void USART1_SendString(const char *str)
{
	while(*str)
	{
		USART1_SendByte((uint8_t)*str++);
	}
}


uint8_t USART1_ReceiveByte(void)
{
	uint16_t data;
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET)
	{}
	data = USART_ReceiveData(USART1);   // 读 DR，顺便清 RXNE
	return (uint8_t)data;  
}

		


