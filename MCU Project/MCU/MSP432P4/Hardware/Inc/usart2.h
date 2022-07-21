#ifndef __USART3_H
#define __USART3_H
#include "sysinit.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32开发板
//串口3驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2009-2019
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define UART2_SEND_LEN_MAX 256                   // uart2 最大发送缓存字节数
extern uint8_t UART2_TX_BUF[UART2_SEND_LEN_MAX]; //发送缓冲,最大UART2_SEND_LEN_MAX字节

void uart2_init(uint32_t baudRate);
void printf_uart2(char *fmt, ...);

#endif
