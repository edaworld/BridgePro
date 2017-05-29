/*
*********************************************************************************************************
*
*   模块名称 : LED模块(For STM32F1XX)
*   文件名称 : bsp_led.c
*   说    明 : 初始化LED灯所需要的GPIO端口
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
*********************************************************************************************************
*   函 数 名: LED_Init
*   功能说明: 初始化LED所对应的引脚，分别为PA11、PA12和PB0、PB1
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;   //对应贴片的绿灯和白灯
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_Pin_RG; //对应红色和绿色的直插LED灯
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
