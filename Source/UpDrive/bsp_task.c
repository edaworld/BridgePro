#include "bsp.h"

#define NULL 0
#define DEVID 3 //机位号码，1，2，3，4等

extern uint8_t g_uart1_timeout; //检测串口1接收数据超时的全局变量，在bsp_slavemsg.c文件中声明
extern uint8_t g_uart2_timeout; //检测串口2接收数据超时的全局变量

extern RECVDATA_T g_tUart1; //初始化从串口1，BLE接收数据结构体，在bsp_slavemsg.c文件中声明
extern RECVDATA_T g_tUart2; //初始化从串口2，Lora接收数据结构体，在bsp_slavemsg.c文件中声明

extern uint8_t TPCTaskNum; //任务数量，在bsp_task.c中被初始化，bsp_tpc.c中使用
//以下为标志位定义
extern uint8_t LoraPinisHigh;  //在main.c中监测lora的IRQ引脚，有状态变化即为接收到lora数据，置位
uint8_t MasterBstisRcv = FALSE; //从主机广播包解析正确，设置的标志位
uint8_t BlEisReady = FALSE; //进入串口，同时接收到了正确的数据，标志位

SLVMSG_T s_tSlaMsg; //STM32发送从机数据的结构体,见bsp_slavemsg.h
uint8_t KeyScan(void); //基于状态机的按键扫描函数
/************************任务结构体说明*************************************/
/**
typedef struct _TPC_TASK
{
	uint8_t   attrb;  //静态任务：0，动态任务：1
	uint8_t   Run;  // 程序运行标记，0：不运行，1：运行xternt
	uint16_t  Timer;  // 计时器
	uint16_t  ItvTime;  // 任务运行间隔时间
	void      (*Task)(void); // 要运行的任务函数
} TPC_TASK; // 任务定义
**/
/************************任务结构体说明*************************************/
TPC_TASK TaskComps[4] =
{
    //添加新任务时，请注意单个任务中改变任务属性的代码
    { 0, 0, 10, 1000, Task_LEDDisplay }, // 静态任务，LED闪烁任务，时间片到达即可执行
    { 0, 0, 1, 2, Task_RecvfromLora }, // 静态任务，处理从SPI接口的SX127 8接收的数据任务，时间片到达即可执行
    { 1, 0, 100, 0, Task_SendToMaster }, // 动态任务，收到广播信号，发送从机数据到主机
    { 0, 0, 1, 10, Task_KeyScan }, // 按键扫描任务
//    { 0, 0, 1, 10, Task_ReadAD5933 }, // 读取AD5933任务    
//    { 0, 0, 1, 1, Task_RecvfromUart }, // 静态任务,通过串口从CC2541接收数据任务    
//	{ 0, 0, 2, 8, Task_PowerCtl }, // 按键扫描任务
//	{ 0, 0, 3, 10, Task_ADCProcess} //采集电池电量任务
};

/*********************************************************************************************************
*   函 数 名: TaskInit
*   功能说明: 任务初始化
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************/
void TaskInit(void)
{
    TPCTaskNum = (sizeof(TaskComps) / sizeof(TaskComps[0])); // 获取任务数
}
/*********************************************************************************************************
*   函 数 名: Task_LEDDisplay
*   功能说明: LED闪烁代码
*********************************************************************************************************/
void Task_LEDDisplay(void)
{
    LED1_TOGGLE(); //白色LED D7
    LCD_P8x16Str ( 0, 2, "LED1_TOGGLE" );
}
/*********************************************************************************************************
*   函 数 名: Task_ReadAD5933
*   功能说明: 读取AD5933电阻抗数据任务
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
            if( temp & 0x02)//实部和虚部有效
                break;
        }
        real = AD5933_Get_Real();
        img  = AD5933_Get_Img();
        AD5933_Set_Mode_Freq_UP();
        printf("$%04X%04X#", real, img);
    }
}

/*********************************************************************************************************
*   函 数 名: Task_ADCProcess
*   功能说明: 采集电池电量任务,10ms调用一次
*********************************************************************************************************/
void Task_ADCProcess(void)
{
	AdcPro(); //通过adc采集电池电量
}
/*********************************************************************************************************
*   函 数 名: Task_KeyScan
*   功能说明: 按键扫描任务
*********************************************************************************************************/
static uint32_t KeyStatus = 0;//存储按键状态变量
void Task_KeyScan(void)
{
    KeyStatus = KeyScan();
}
/*********************************************************************************************************
*   函 数 名: Task_PowerCtl
*   功能说明: PWR控制电源引脚，控制电源关闭的任务
*********************************************************************************************************/
void Task_PowerCtl(void)
{  
    if(KeyStatus == 2)
    {
        KEY_PWR =  0;//关闭电源
    }
}

/*********************************************************************************************************
*   函 数 名: Task_RecvfromLora
*   功能说明: 处理从SPI接口的Lora接收的数据任务
*********************************************************************************************************/

static uint8_t recvdatbuffer[32];   //接收数据缓冲区
//static uint8_t recvprintbuffer[32]; //接收数据打印缓冲区
//static uint8_t ucrevcount;
void Task_RecvfromLora(void)
{
//    uint8_t length;    
    if (LoraPinisHigh  == TRUE)//主函数中检测到中断引脚为高，表示接收到数据后，置位该标志位
    {
//		OLEDPrint(0, 2, "RF Received");
//		sprintf(sendBuf, "连续发送:%d", bsp_GetRunTime());
//		OLEDPrint(0, 2, sendBuf);
//		printf("\t%d\n", bsp_GetRunTime()); //测试超时时间
//		以下两个操作耗时8ms
//        length = RFM96_LoRaRxPacket(recvdatbuffer);
        RFM96_LoRaRxPacket(recvdatbuffer);
        RFRxMode();
//		以上两个操作耗时8ms
//		printf("\t%d\n", bsp_GetRunTime()); //测试超时时间
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
            MasterBstisRcv = TRUE;   //设置接收到的主机包标志位
            TaskComps[2].attrb = 0; //将节点发送任务设置为静态任务
            TaskComps[2].Timer = (DEVID-1) * 100 + 5; //节点1，收到广播信号后1ms后启动发送任务,节点2，即为101ms。
//            OLEDPrint(0,3,"recv master!");
        }
        LoraPinisHigh = FALSE;
    }  
}

/*********************************************************************************************************
*   函 数 名: Task_SendToMaster
*   功能说明: 发送数据包至主机任务
*********************************************************************************************************/
void Task_SendToMaster(void)
{
    if ((MasterBstisRcv == TRUE) && (BlEisReady == TRUE)) //接收到广播信号，并且从2541通过串口接收到数据
    {
        //节点赋值
        s_tSlaMsg.head = '&';
        s_tSlaMsg.devID = DEVID;
        s_tSlaMsg.BatPowerdata = GetADC()*100/2606;//从ADC0通道读取数据进行换算
//        s_tSlaMsg.Heartdata = 64;
//        s_tSlaMsg.HrtPowerdata = 99;
        s_tSlaMsg.tail = '%';
        RFSendData(s_tSlaMsg.msg, 6); //发送该节点数据
        mem_set(s_tSlaMsg.msg,0,6); //发送完毕后将结构体数据清零
        TaskComps[2].attrb = 1; //将发送节点数据任务设置为动态任务，等待再次接收到广播信号
        MasterBstisRcv = FALSE;
        BlEisReady = FALSE;
    }
}
/*********************************************************************************************************
*   函 数 名: Task_RecvfromUart
*   功能说明: 处理从uart1接口接收到的CC2541发送过来的数据任务
*********************************************************************************************************/
void Task_RecvfromUart(void)
{

//超过3.5个字符时间后执行Uart1_RxTimeOut函数。全局变量 g_uart1_timeout = 1; 通知主程序开始解码
	if (g_uart1_timeout == 0)
	{
		return; // 没有超时，继续接收。不要清零 g_tUart1.RxCount
	}
	if (g_tUart1.RxCount < 5)    // 接收到的数据小于3个字节就认为错误
	{
		return;
	}
//    printf("%d",g_tUart1.RxCount); //测试接收数据个数是否正确
	g_uart1_timeout = 0; // 超时清标志
//    printf("\t%d\n",bsp_GetRunTime());//测试超时时间
	if ((g_tUart1.RxBuf[0] != '$') && (g_tUart1.RxBuf[4] != '#')) //检测数据包头是否正确
	{
		printf("error in head!");
	} 
    else if (g_tUart1.RxBuf[1] == 'P' || g_tUart1.RxBuf[1] == 'H') //检测数据包是否都正确
	{
//数据包接收正确
		s_tSlaMsg.Heartdata = g_tUart1.RxBuf[2];//心率数值
		s_tSlaMsg.HrtPowerdata = g_tUart1.RxBuf[3];//心率带电池电量
        BlEisReady = TRUE;
	} 
    else
	{
//		s_tSlaMsg.Heartdata = 0;
//		s_tSlaMsg.HrtPowerdata = 0;
	}
	g_tUart1.RxCount = 0; // 必须清零计数器，方便下次帧同步
}


/*********************************************************************************************************
*   函 数 名: KeyScan
*   功能说明: 按键扫描代码，按键接到PA4引脚，当按下按键超过3秒钟的时候
*********************************************************************************************************/
uint8_t KeyScan(void)
{
	static uint8_t ucKeyStatus = 0; //表示按键状态，0表示未按下，1表示按下，2表示长按，3表示弹起
	static uint32_t uiKeyInput = 0; //保存读取的按键键值，按键按下时为高电平
	static uint32_t uiCount = 0; //长按计数器，长按三秒开机，应计数100次
	uint8_t key_return = 0; //函数返回值
	uiKeyInput = KEY_KEY;

	switch (ucKeyStatus)
	{
	case 0: //如果是初始态，即无动作
		if (uiKeyInput == 0)
		{
			ucKeyStatus = 1;
		} else
		{
			ucKeyStatus = 0;
		}
		break;
	case 1: //如果先前是被按着
		if (uiKeyInput == 0) //如果现在还被按着
		{
			ucKeyStatus = 2; //切换到计时态
			uiCount = 0;
		} else
		{
			ucKeyStatus = 0;
		}
		break;
	case 2: //如果已经切换到状态2
		if (uiKeyInput == 0) //如果现在还被按着
		{
			ucKeyStatus = 2;
			uiCount++;
			key_return = 1; //返回1,一次完整的普通按键，程序进入这个语句块，说明已经有2次以上10ms的中断，等于已经消抖
							//那么此时检测到按键被释放，说明是一次普通短按
			if (uiCount > 300) //按键时间大于3s
			{
				uiCount = 0;
				key_return = 2; //长按情况
			}
		} else //如果已经弹起
		{
			ucKeyStatus = 3;
		}
		break;
	case 3: //如果已经切换到状态3
		if (uiKeyInput == 1)//如果已经弹起
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
