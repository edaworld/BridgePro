#ifndef __OLED_H__
#define __OLED_H__

/*----------------------------------------------------------------------------*
 * 包含头文件                                                                 *
 *----------------------------------------------------------------------------*/

#include "stm32f10x.h"
/*----------------------------------------------------------------------------*
 * 宏定义                                                                     *
 *----------------------------------------------------------------------------*/
#define LED_IMAGE_WHITE       1
#define LED_IMAGE_BLACK       0

#define LED_MAX_ROW_NUM      64
#define LED_MAX_COLUMN_NUM  128

/*----------------------------------------------------------------------------*
 * 全局变量                                                                   *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * 内部函数原型                                                               *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * 外部函数原型                                                               *
 *----------------------------------------------------------------------------*/
extern void OLEDIO_Init(void); 
extern void OLED_Init(void);
extern void OLED_Fill(uint8_t ucData);
extern void OLED_SetPos(uint8_t ucIdxX, uint8_t ucIdxY); 
extern void OLED_P6x8Char(uint8_t ucIdxX,uint8_t ucIdxY,uint8_t ucData);   //显示一个6x8标准ASCII字符
extern void OLED_P6x8Str(uint8_t ucIdxX,uint8_t ucIdxY,int8_t ucDataStr[]);
extern void OLED_P8x16Str(uint8_t ucIdxX,uint8_t ucIdxY,int8_t ucDataStr[]);  //写入一组8x16标准ASCII字符串
extern void OLED_P14x16Str(uint8_t ucIdxX,uint8_t ucIdxY,int8_t ucDataStr[]);	  //写入一组14x16的中文字符串（字符串表格中需含有此字）
extern void OLED_PXx16MixStr(uint8_t ucIdxX, uint8_t ucIdxY, int8_t ucDataStr[]); //输出14x16汉字和字符混合字符串 （字符串表格中需含有此字）
extern void OLED_Fill(uint8_t ucData);
extern void OLED_PrintChar(uint8_t ucIdxX, uint8_t ucIdxY, int8_t cData);
extern void OLED_PrintShort(uint8_t ucIdxX, uint8_t ucIdxY, int16_t sData);
extern void OLED_PrintImage(uint8_t *pucTable, uint16_t usRowNum, uint16_t usColumnNum);

extern void OLEDPrint(int x,int y,char *msg);
#endif

