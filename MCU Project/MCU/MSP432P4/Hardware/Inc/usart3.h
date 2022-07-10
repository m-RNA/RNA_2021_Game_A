#ifndef __USART3_H
#define __USART3_H
#include "sysinit.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口3驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define HC_05_USART_BASE EUSCI_A2_BASE
#define HC_05_USART_PORT_PIN GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3

#define USART3_MAX_SEND_LEN 600 //最大发送缓存字节数
#define USART3_RX_EN 1          //0,不接收;1,接收.

extern uint8_t USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节
extern volatile uint16_t USART3_RX_STA;            //接收数据状态

void usart3_init(uint32_t baudRate); //串口2初始化
void u3_printf(char *fmt, ...);
void BluetoothSendDate(float *gyh, float THDx, uint16_t *waveBin);

#endif
