/*
*********************************************************************************************************
*
*   ģ������ : LEDģ��(For STM32F1XX)
*   �ļ����� : bsp_led.c
*   ˵    �� : ��ʼ��LED������Ҫ��GPIO�˿�
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
*********************************************************************************************************
*   �� �� ��: LED_Init
*   ����˵��: ��ʼ��LED����Ӧ�����ţ��ֱ�ΪPA11��PA12��PB0��PB1
*   ��    �Σ���
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;   //��Ӧ��Ƭ���̵ƺͰ׵�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_Pin_RG; //��Ӧ��ɫ����ɫ��ֱ��LED��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
}
