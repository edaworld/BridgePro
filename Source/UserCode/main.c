#include "bsp.h"  // Device header

uint8_t LoraPinisHigh = FALSE;//�յ����ݱ�־λ��

int main(void)
{
	bsp_Init();  //��ʼ��Ӳ���豸   
    bsp_DelayMS(10);
    TaskInit(); //��ʼ������,���ǻ�ȡ�������������������bsp_idle�е���task_processʵ��    

    LED_GREEN = 1;
    LED_SMT_GREEN = 1;
    LED_RED = 1;
    LED_SMT_WHITE = 1;
	while (1)
	{
//		if (GPIO_ReadInputDataBit(GPIOB, RF_IRQ_PIN)) //SPI��SX1278�����յ�����ʱ��IRQ����Ϊ��
//		{
//			LoraPinisHigh = TRUE;
//		}
		bsp_Idle();       
	}
}
