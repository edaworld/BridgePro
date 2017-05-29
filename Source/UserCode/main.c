#include "bsp.h"  // Device header

#define EXAMPLE_NAME  "���ں�PC��ͨ�ţ������жϡ�FIFO���ƣ�"
#define DEMO_VER  "1.0"
#define MSRID  "01"

static void printfLogo(void);
uint8_t LoraPinisHigh = FALSE;//�յ����ݱ�־λ��
/*********************************************************************************************************
*   �� �� ��: bsp_StartMachine
*   ����˵��: �������������ڹػ�״̬��ʱ�򣬰��°���������1���ӿ���
*   ��    ��: ��
*   �� �� ֵ: ��
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
            if(++i > 10)//��������1���ӣ�����
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
	bsp_Init();  //��ʼ��Ӳ���豸
    printfLogo();    
    bsp_DelayMS(10);
    TaskInit(); //��ʼ������,���ǻ�ȡ�������������������bsp_idle�е���task_processʵ��    

    LED_GREEN = 1;
    LED_SMT_GREEN = 1;
    LED_RED = 1;
    LED_SMT_WHITE = 1;
    bsp_StartMachine();
	while (1)
	{
		if (GPIO_ReadInputDataBit(GPIOB, RF_IRQ_PIN)) //SPI��SX1278�����յ�����ʱ��IRQ����Ϊ��
		{
			LoraPinisHigh = TRUE;
		}
		bsp_Idle();       
	}
}

static void printfLogo(void)
{
	printf("********************************************************\r\n");
	printf("* �ӻ���IDΪ : %s\r\n", MSRID);   /* ��ӡ�������� */
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);  /* ��ӡ�������� */
	printf("* ����汾   : %s\r\n", DEMO_VER);      /* ��ӡ����汾 */
	printf("* ��������   : %s\r\n", __DATE__);  /* ��ӡ�������� */
	printf("* ��������   : %s\r\n", __TIME__);  /* ��ӡ�������� */
	/* ��ӡST�̼���汾���궨���� stm32f10x.h �ļ� */
	printf("* �̼���汾 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN, __STM32F10X_STDPERIPH_VERSION_SUB1, __STM32F10X_STDPERIPH_VERSION_SUB2);
	/* ��ӡ CMSIS �汾. �궨���� core_cm3.h �ļ� */
	printf("* CMSIS�汾  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);
	printf("********************************************************\r\n");

}
