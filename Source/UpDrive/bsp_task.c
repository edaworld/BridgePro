#include "bsp.h"

#define NULL 0
#define DEVID 3 //��λ���룬1��2��3��4��

extern uint8_t g_uart1_timeout; //��⴮��1�������ݳ�ʱ��ȫ�ֱ�������bsp_slavemsg.c�ļ�������
extern uint8_t g_uart2_timeout; //��⴮��2�������ݳ�ʱ��ȫ�ֱ���

extern RECVDATA_T g_tUart1; //��ʼ���Ӵ���1��BLE�������ݽṹ�壬��bsp_slavemsg.c�ļ�������
extern RECVDATA_T g_tUart2; //��ʼ���Ӵ���2��Lora�������ݽṹ�壬��bsp_slavemsg.c�ļ�������

extern uint8_t TPCTaskNum; //������������bsp_task.c�б���ʼ����bsp_tpc.c��ʹ��
//����Ϊ��־λ����
extern uint8_t LoraPinisHigh;  //��main.c�м��lora��IRQ���ţ���״̬�仯��Ϊ���յ�lora���ݣ���λ
uint8_t MasterBstisRcv = FALSE; //�������㲥��������ȷ�����õı�־λ
uint8_t BlEisReady = FALSE; //���봮�ڣ�ͬʱ���յ�����ȷ�����ݣ���־λ

SLVMSG_T s_tSlaMsg; //STM32���ʹӻ����ݵĽṹ��,��bsp_slavemsg.h
uint8_t KeyScan(void); //����״̬���İ���ɨ�躯��
/************************����ṹ��˵��*************************************/
/**
typedef struct _TPC_TASK
{
	uint8_t   attrb;  //��̬����0����̬����1
	uint8_t   Run;  // �������б�ǣ�0�������У�1������xternt
	uint16_t  Timer;  // ��ʱ��
	uint16_t  ItvTime;  // �������м��ʱ��
	void      (*Task)(void); // Ҫ���е�������
} TPC_TASK; // ������
**/
/************************����ṹ��˵��*************************************/
TPC_TASK TaskComps[4] =
{
    //���������ʱ����ע�ⵥ�������иı��������ԵĴ���
    { 0, 0, 10, 1000, Task_LEDDisplay }, // ��̬����LED��˸����ʱ��Ƭ���Ｔ��ִ��
    { 0, 0, 1, 2, Task_RecvfromLora }, // ��̬���񣬴����SPI�ӿڵ�SX127 8���յ���������ʱ��Ƭ���Ｔ��ִ��
    { 1, 0, 100, 0, Task_SendToMaster }, // ��̬�����յ��㲥�źţ����ʹӻ����ݵ�����
    { 0, 0, 1, 10, Task_KeyScan }, // ����ɨ������
//    { 0, 0, 1, 10, Task_ReadAD5933 }, // ��ȡAD5933����    
//    { 0, 0, 1, 1, Task_RecvfromUart }, // ��̬����,ͨ�����ڴ�CC2541������������    
//	{ 0, 0, 2, 8, Task_PowerCtl }, // ����ɨ������
//	{ 0, 0, 3, 10, Task_ADCProcess} //�ɼ���ص�������
};

/*********************************************************************************************************
*   �� �� ��: TaskInit
*   ����˵��: �����ʼ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************/
void TaskInit(void)
{
    TPCTaskNum = (sizeof(TaskComps) / sizeof(TaskComps[0])); // ��ȡ������
}
/*********************************************************************************************************
*   �� �� ��: Task_LEDDisplay
*   ����˵��: LED��˸����
*********************************************************************************************************/
void Task_LEDDisplay(void)
{
    LED1_TOGGLE(); //��ɫLED D7
    LCD_P8x16Str ( 0, 2, "LED1_TOGGLE" );
}
/*********************************************************************************************************
*   �� �� ��: Task_ReadAD5933
*   ����˵��: ��ȡAD5933���迹��������
*********************************************************************************************************/
static unsigned char temp;
static unsigned int real, img;
void Task_ReadAD5933(void)
{
    AD5933_Set_Mode_Freq_Start();
    if((AD5933_Get_DFT_ST() & 0x04) != 0x04)
    {
        while(1)//wait for DFT finish
        {
            temp = AD5933_Get_DFT_ST();
            if( temp & 0x02)//ʵ�����鲿��Ч
                break;
        }
        real = AD5933_Get_Real();
        img  = AD5933_Get_Img();
        AD5933_Set_Mode_Freq_UP();
        printf("$%04X%04X#", real, img);
    }
}

/*********************************************************************************************************
*   �� �� ��: Task_ADCProcess
*   ����˵��: �ɼ���ص�������,10ms����һ��
*********************************************************************************************************/
void Task_ADCProcess(void)
{
	AdcPro(); //ͨ��adc�ɼ���ص���
}
/*********************************************************************************************************
*   �� �� ��: Task_KeyScan
*   ����˵��: ����ɨ������
*********************************************************************************************************/
static uint32_t KeyStatus = 0;//�洢����״̬����
void Task_KeyScan(void)
{
    KeyStatus = KeyScan();
}
/*********************************************************************************************************
*   �� �� ��: Task_PowerCtl
*   ����˵��: PWR���Ƶ�Դ���ţ����Ƶ�Դ�رյ�����
*********************************************************************************************************/
void Task_PowerCtl(void)
{  
    if(KeyStatus == 2)
    {
        KEY_PWR =  0;//�رյ�Դ
    }
}

/*********************************************************************************************************
*   �� �� ��: Task_RecvfromLora
*   ����˵��: �����SPI�ӿڵ�Lora���յ���������
*********************************************************************************************************/

static uint8_t recvdatbuffer[32];   //�������ݻ�����
//static uint8_t recvprintbuffer[32]; //�������ݴ�ӡ������
//static uint8_t ucrevcount;
void Task_RecvfromLora(void)
{
//    uint8_t length;    
    if (LoraPinisHigh  == TRUE)//�������м�⵽�ж�����Ϊ�ߣ���ʾ���յ����ݺ���λ�ñ�־λ
    {
//		OLEDPrint(0, 2, "RF Received");
//		sprintf(sendBuf, "��������:%d", bsp_GetRunTime());
//		OLEDPrint(0, 2, sendBuf);
//		printf("\t%d\n", bsp_GetRunTime()); //���Գ�ʱʱ��
//		��������������ʱ8ms
//        length = RFM96_LoRaRxPacket(recvdatbuffer);
        RFM96_LoRaRxPacket(recvdatbuffer);
        RFRxMode();
//		��������������ʱ8ms
//		printf("\t%d\n", bsp_GetRunTime()); //���Գ�ʱʱ��
//		if (length > 0)
//		{
//			ucrevcount++;
//			sprintf(recvprintbuffer, "rcvcnt is :%d", ucrevcount);
//		}
//		OLEDPrint(0, 2, recvprintbuffer);
//		printf("receive data length is %d\n", length);
//		COMx_SendBuf(COM1, recvdatbuffer, 6);
        if ((recvdatbuffer[0] == '$') && (recvdatbuffer[1] == '#') && (recvdatbuffer[2] == 'S') && (recvdatbuffer[3] == 'T'))
        {
            MasterBstisRcv = TRUE;   //���ý��յ�����������־λ
            TaskComps[2].attrb = 0; //���ڵ㷢����������Ϊ��̬����
            TaskComps[2].Timer = (DEVID-1) * 100 + 5; //�ڵ�1���յ��㲥�źź�1ms��������������,�ڵ�2����Ϊ101ms��
//            OLEDPrint(0,3,"recv master!");
        }
        LoraPinisHigh = FALSE;
    }  
}

/*********************************************************************************************************
*   �� �� ��: Task_SendToMaster
*   ����˵��: �������ݰ�����������
*********************************************************************************************************/
void Task_SendToMaster(void)
{
    if ((MasterBstisRcv == TRUE) && (BlEisReady == TRUE)) //���յ��㲥�źţ����Ҵ�2541ͨ�����ڽ��յ�����
    {
        //�ڵ㸳ֵ
        s_tSlaMsg.head = '&';
        s_tSlaMsg.devID = DEVID;
        s_tSlaMsg.BatPowerdata = GetADC()*100/2606;//��ADC0ͨ����ȡ���ݽ��л���
//        s_tSlaMsg.Heartdata = 64;
//        s_tSlaMsg.HrtPowerdata = 99;
        s_tSlaMsg.tail = '%';
        RFSendData(s_tSlaMsg.msg, 6); //���͸ýڵ�����
        mem_set(s_tSlaMsg.msg,0,6); //������Ϻ󽫽ṹ����������
        TaskComps[2].attrb = 1; //�����ͽڵ�������������Ϊ��̬���񣬵ȴ��ٴν��յ��㲥�ź�
        MasterBstisRcv = FALSE;
        BlEisReady = FALSE;
    }
}
/*********************************************************************************************************
*   �� �� ��: Task_RecvfromUart
*   ����˵��: �����uart1�ӿڽ��յ���CC2541���͹�������������
*********************************************************************************************************/
void Task_RecvfromUart(void)
{

//����3.5���ַ�ʱ���ִ��Uart1_RxTimeOut������ȫ�ֱ��� g_uart1_timeout = 1; ֪ͨ������ʼ����
	if (g_uart1_timeout == 0)
	{
		return; // û�г�ʱ���������ա���Ҫ���� g_tUart1.RxCount
	}
	if (g_tUart1.RxCount < 5)    // ���յ�������С��3���ֽھ���Ϊ����
	{
		return;
	}
//    printf("%d",g_tUart1.RxCount); //���Խ������ݸ����Ƿ���ȷ
	g_uart1_timeout = 0; // ��ʱ���־
//    printf("\t%d\n",bsp_GetRunTime());//���Գ�ʱʱ��
	if ((g_tUart1.RxBuf[0] != '$') && (g_tUart1.RxBuf[4] != '#')) //������ݰ�ͷ�Ƿ���ȷ
	{
		printf("error in head!");
	} 
    else if (g_tUart1.RxBuf[1] == 'P' || g_tUart1.RxBuf[1] == 'H') //������ݰ��Ƿ���ȷ
	{
//���ݰ�������ȷ
		s_tSlaMsg.Heartdata = g_tUart1.RxBuf[2];//������ֵ
		s_tSlaMsg.HrtPowerdata = g_tUart1.RxBuf[3];//���ʴ���ص���
        BlEisReady = TRUE;
	} 
    else
	{
//		s_tSlaMsg.Heartdata = 0;
//		s_tSlaMsg.HrtPowerdata = 0;
	}
	g_tUart1.RxCount = 0; // ��������������������´�֡ͬ��
}


/*********************************************************************************************************
*   �� �� ��: KeyScan
*   ����˵��: ����ɨ����룬�����ӵ�PA4���ţ������°�������3���ӵ�ʱ��
*********************************************************************************************************/
uint8_t KeyScan(void)
{
	static uint8_t ucKeyStatus = 0; //��ʾ����״̬��0��ʾδ���£�1��ʾ���£�2��ʾ������3��ʾ����
	static uint32_t uiKeyInput = 0; //�����ȡ�İ�����ֵ����������ʱΪ�ߵ�ƽ
	static uint32_t uiCount = 0; //�������������������뿪����Ӧ����100��
	uint8_t key_return = 0; //��������ֵ
	uiKeyInput = KEY_KEY;

	switch (ucKeyStatus)
	{
	case 0: //����ǳ�ʼ̬�����޶���
		if (uiKeyInput == 0)
		{
			ucKeyStatus = 1;
		} else
		{
			ucKeyStatus = 0;
		}
		break;
	case 1: //�����ǰ�Ǳ�����
		if (uiKeyInput == 0) //������ڻ�������
		{
			ucKeyStatus = 2; //�л�����ʱ̬
			uiCount = 0;
		} else
		{
			ucKeyStatus = 0;
		}
		break;
	case 2: //����Ѿ��л���״̬2
		if (uiKeyInput == 0) //������ڻ�������
		{
			ucKeyStatus = 2;
			uiCount++;
			key_return = 1; //����1,һ����������ͨ�������������������飬˵���Ѿ���2������10ms���жϣ������Ѿ�����
							//��ô��ʱ��⵽�������ͷţ�˵����һ����ͨ�̰�
			if (uiCount > 300) //����ʱ�����3s
			{
				uiCount = 0;
				key_return = 2; //�������
			}
		} else //����Ѿ�����
		{
			ucKeyStatus = 3;
		}
		break;
	case 3: //����Ѿ��л���״̬3
		if (uiKeyInput == 1)//����Ѿ�����
		{
			ucKeyStatus = 0;
			uiCount = 0;
		}
		break;
	default:
		ucKeyStatus = 0;
		uiCount = 0;
	}
	return key_return;
}
