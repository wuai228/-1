#include "stm32f10x.h"                  // Device header
#include "string.h"
#include <stdio.h>

void I2C1_Init()
{
	GPIO_InitTypeDef gpio;
	I2C_InitTypeDef i2c;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PB6 SCL   PB7 SDA;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gpio);
	
	
	I2C_SoftwareResetCmd(I2C1,ENABLE);
	I2C_SoftwareResetCmd(I2C1,DISABLE);
	
	I2C_DeInit(I2C1);
	
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_OwnAddress1 = 0x00;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c.I2C_ClockSpeed = 100000;
	
	I2C_Init(I2C1,&i2c);
	I2C_Cmd(I2C1,ENABLE);
}
int I2C1_CheckAddr(uint8_t dev7)
{
    // START
    I2C_GenerateSTART(I2C1, ENABLE);

    uint32_t t = 200000;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) && --t);
    if (t == 0) {
        printf("START timeout\r\n");
        I2C_GenerateSTOP(I2C1, ENABLE);
        return 0;
    }

    // send addr
    I2C_Send7bitAddress(I2C1, (uint8_t)(dev7 << 1), I2C_Direction_Transmitter);

    t = 200000;
    while (((I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0) && --t);
    if (t == 0) {
        printf("ADDR wait timeout, SR1=0x%04X\r\n", I2C1->SR1);
        I2C_GenerateSTOP(I2C1, ENABLE);
        return 0;
    }

    if (I2C1->SR1 & I2C_SR1_ADDR) {
        (void)I2C1->SR1; (void)I2C1->SR2;
        I2C_GenerateSTOP(I2C1, ENABLE);
        return 1;
    }

    if (I2C1->SR1 & I2C_SR1_AF) {
        I2C1->SR1 &= ~I2C_SR1_AF;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}
