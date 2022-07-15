/******************************************************************************
//MSP432P401R
//GPIO配置 (LED KEY BEEP OLED)
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/8/12
*******************************************************************************/

#ifndef __BASEBOARD_H
#define __BASEBOARD_H
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "draw_api.h"
#include "led.h"

#define KEY1_OnBoard BITBAND_PERI(P1IN, 1) //读取按键1
#define KEY2_OnBoard BITBAND_PERI(P1IN, 4) //读取按键2

#define KEY1 BITBAND_PERI(P5IN, 5) //读取按键1
#define KEY2 BITBAND_PERI(P5IN, 1) //读取按键2
#define KEY3 BITBAND_PERI(P5IN, 2) //读取按键3
#define KEY4 BITBAND_PERI(P5IN, 4) //读取按键4

#define BEEP BITBAND_PERI(P5OUT,0) 

enum KEY_PRES
{
	NOT_PRES = 0,
    KEY1_OnBoard_PRES,
    KEY2_OnBoard_PRES,
    KEY1_PRES,
    KEY2_PRES,
    KEY3_PRES,
    KEY4_PRES,
};

void KEY_Init(void);
uint8_t KEY_Scan(bool mode);

void BaseBoardInit(void);    //IO初始化

#endif
