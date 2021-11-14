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

/****************************************************/
// MSP432P401R
// 串口2配置
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
/****************************************************/

#include "usart3.h"
#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timA.h"
#include "baudrate_calculate.h"

/* 发送给手机 */
void BluetoothSendDate(float *gyh, float THDx, uint16_t *waveBin)
{
	uint8_t i;
	/* 发送THD */
	MAP_UART_transmitData(HC_05_USART_BASE, (uint16_t)(THDx * 100) >> 8);
	MAP_UART_transmitData(HC_05_USART_BASE, (uint16_t)(THDx * 100) & 0xff);

	/* 发送拟合值 */
	for (i = 0; i < 128; ++i)
	{
		MAP_UART_transmitData(HC_05_USART_BASE, waveBin[i] >> 8);
		MAP_UART_transmitData(HC_05_USART_BASE, waveBin[i] & 0xff);
	}

	/* 发送归一化幅值 */
	for (i = 0; i < 4; ++i)
	{
		MAP_UART_transmitData(HC_05_USART_BASE, (uint16_t)(gyh[i] * 100) >> 8);
		MAP_UART_transmitData(HC_05_USART_BASE, (uint16_t)(gyh[i] * 100) & 0xff);
	}

}

//串口接收缓存区
uint8_t USART3_TX_BUF[USART3_MAX_SEND_LEN]; //发送缓冲,最大USART3_MAX_SEND_LEN字节


//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void usart3_init(uint32_t baudRate)
{
#ifdef EUSCI_A_UART_7_BIT_LEN
	//固件库v3_40_01_02
	//默认SMCLK 48MHz 比特率 115200
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
	//默认SMCLK 48MHz 比特率 115200
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
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(HC_05_USART_PORT_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_UART_initModule(HC_05_USART_BASE, &uartConfig);
	MAP_UART_enableModule(HC_05_USART_BASE);
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char *fmt, ...)
{
	uint16_t i, j;
	va_list ap;
	va_start(ap, fmt);
	vsprintf((char *)USART3_TX_BUF, fmt, ap);
	va_end(ap);
	i = strlen((const char *)USART3_TX_BUF); //此次发送数据的长度
	for (j = 0; j < i; j++)					 //循环发送数据
	{
		MAP_UART_transmitData(HC_05_USART_BASE, USART3_TX_BUF[j]);
	}
}
