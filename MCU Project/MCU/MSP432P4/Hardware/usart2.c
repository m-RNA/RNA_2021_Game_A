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

/****************************************************/
// MSP432P401R
// 串口2配置
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
/****************************************************/

#include "usart2.h"
#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timA.h"
#include "baudrate_calculate.h"

//串口接收缓存区
uint8_t UART2_TX_BUF[UART2_SEND_LEN_MAX]; //发送缓冲,最大UART2_SEND_LEN_MAX字节

//初始化IO 串口2
// bound:波特率
void uart2_init(uint32_t baudRate)
{
#ifdef EUSCI_A_UART_7_BIT_LEN
	//固件库v3_40_01_02
	//默认SMCLK 48MHz 比特率
	const eUSCI_UART_ConfigV1 uartConfig =
		{
			EUSCI_A_UART_CLOCKSOURCE_SMCLK,				   // SMCLK Clock Source
			312,										   // BRDIV = 312
			8,											   // UCxBRF = 8
			1,											   // UCxBRS = 1
			EUSCI_A_UART_NO_PARITY,						   // No Parity
			EUSCI_A_UART_LSB_FIRST,						   // MSB First
			EUSCI_A_UART_ONE_STOP_BIT,					   // One stop bit
			EUSCI_A_UART_MODE,							   // UART mode
			EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
			EUSCI_A_UART_8_BIT_LEN						   // 8 bit data length
		};
	eusci_calcBaudDividers((eUSCI_UART_ConfigV1 *)&uartConfig, baudRate); //配置波特率
#else
	//固件库v3_21_00_05
	//默认SMCLK 48MHz 比特率
	const eUSCI_UART_Config uartConfig =
		{
			EUSCI_A_UART_CLOCKSOURCE_SMCLK,				   // SMCLK Clock Source
			312,										   // BRDIV = 312
			8,											   // UCxBRF = 8
			1,											   // UCxBRS = 1
			EUSCI_A_UART_NO_PARITY,						   // No Parity
			EUSCI_A_UART_LSB_FIRST,						   // MSB First
			EUSCI_A_UART_ONE_STOP_BIT,					   // One stop bit
			EUSCI_A_UART_MODE,							   // UART mode
			EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
		};
	eusci_calcBaudDividers((eUSCI_UART_Config *)&uartConfig, baudRate); //配置波特率
#endif
	// 1.配置GPIO复用
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig); // 3.初始化串口
	MAP_UART_enableModule(EUSCI_A2_BASE);			 // 4.开启串口模块
}

//串口2,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void printf_uart2(char *fmt, ...)
{
	uint16_t i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)UART2_TX_BUF, fmt, ap);
	va_end(ap);
	i = strlen((const char *)UART2_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					//循环发送数据
	{
		MAP_UART_transmitData(EUSCI_A2_BASE, UART2_TX_BUF[j]);
	}
}
