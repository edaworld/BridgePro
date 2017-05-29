#include "bsp.h"  // Device header

#define EXAMPLE_NAME  "串口和PC机通信（串口中断、FIFO机制）"
#define DEMO_VER  "1.0"
#define MSRID  "01"

static void printfLogo(void);
uint8_t LoraPinisHigh = FALSE;//收到数据标志位，
/*********************************************************************************************************
*   函 数 名: bsp_StartMachine
*   功能说明: 开机函数，处于关机状态下时候，按下按键，超过1秒钟开机
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************/
void bsp_StartMachine(void)
{
    uint8_t i = 0;
    uint32_t temp1;
    uint32_t temp2;
    temp1 = KEY_KEY;
    if(temp1 == 0)
    {
        while(1)
        {
            bsp_DelayMS(100);
            if(++i > 10)//长按超过1秒钟，开机
            {
                KEY_PWR = 1;
                break;
            }
            else
            {
                temp2 = KEY_KEY;
                if(temp2 !=0)
                    i=0;
            }
        }
    }
}

int main(void)
{
	bsp_Init();  //初始化硬件设备
    printfLogo();    
    bsp_DelayMS(10);
    TaskInit(); //初始化任务,仅是获取任务数量，任务调度在bsp_idle中调用task_process实现    

    LED_GREEN = 1;
    LED_SMT_GREEN = 1;
    LED_RED = 1;
    LED_SMT_WHITE = 1;
    bsp_StartMachine();
	while (1)
	{
		if (GPIO_ReadInputDataBit(GPIOB, RF_IRQ_PIN)) //SPI的SX1278，接收到数据时，IRQ引脚为高
		{
			LoraPinisHigh = TRUE;
		}
		bsp_Idle();       
	}
}

static void printfLogo(void)
{
	printf("********************************************************\r\n");
	printf("* 从机，ID为 : %s\r\n", MSRID);   /* 打印程序名称 */
	printf("* 程序名称   : %s\r\n", EXAMPLE_NAME);  /* 打印程序名称 */
	printf("* 程序版本   : %s\r\n", DEMO_VER);      /* 打印程序版本 */
	printf("* 发布日期   : %s\r\n", __DATE__);  /* 打印程序日期 */
	printf("* 发布日期   : %s\r\n", __TIME__);  /* 打印程序日期 */
	/* 打印ST固件库版本，宏定义在 stm32f10x.h 文件 */
	printf("* 固件库版本 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN, __STM32F10X_STDPERIPH_VERSION_SUB1, __STM32F10X_STDPERIPH_VERSION_SUB2);
	/* 打印 CMSIS 版本. 宏定义在 core_cm3.h 文件 */
	printf("* CMSIS版本  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);
	printf("********************************************************\r\n");

}
