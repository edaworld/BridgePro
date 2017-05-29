#ifndef  __LED_H
#define  __LED_H
#include "stm32f10x.h" // Device header

/****************************以下为Heartbag V2.0************************************/
#define     LED_SMT_GREEN         PAout(11) //PA11,绿色LED D8
#define     LED_SMT_WHITE         PAout(12) //PA12,白色LED D7
#define     LED_Pin_GW            GPIO_Pin_11 | GPIO_Pin_12

#define     LED_GREEN             PBout(0)   //PB0,红色直插LED D10
#define     LED_RED               PBout(1)   //PB1,绿色直插LED D9
#define     LED_Pin_RG            GPIO_Pin_0 | GPIO_Pin_1


#define		LedRedOn()      GPIO_ResetBits(GPIOB,GPIO_Pin_0)	
#define		LedRedOff()     GPIO_SetBits(GPIOB,GPIO_Pin_0)	
#define		LedGreenOn()    GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define		LedGreenOff()   GPIO_SetBits(GPIOB,GPIO_Pin_1)

void        LED_Init(void); //LED所对应的IO口初始化函数
#endif
