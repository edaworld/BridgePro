#include "bsp.h"  // Device header

uint8_t LoraPinisHigh = FALSE;//收到数据标志位，

int main(void)
{
	bsp_Init();  //初始化硬件设备   
    bsp_DelayMS(10);
    TaskInit(); //初始化任务,仅是获取任务数量，任务调度在bsp_idle中调用task_process实现    

    LED_GREEN = 1;
    LED_SMT_GREEN = 1;
    LED_RED = 1;
    LED_SMT_WHITE = 1;
	while (1)
	{
//		if (GPIO_ReadInputDataBit(GPIOB, RF_IRQ_PIN)) //SPI的SX1278，接收到数据时，IRQ引脚为高
//		{
//			LoraPinisHigh = TRUE;
//		}
		bsp_Idle();       
	}
}
