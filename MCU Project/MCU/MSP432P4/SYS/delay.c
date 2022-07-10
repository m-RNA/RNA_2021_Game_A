/*******************************************
//MSP432P401R
//4 移植滴答延时
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/7/22
*******************************************/

/********      正点原子@ALIENTEK      ********
 * 技术论坛:www.openedv.com
 * 创建日期:2010/1/1
 * 版本：V1.8
 *
 * 说明 ：
 * 源码为正点原子@ALIENTEK 编写
 * 本人RNA，只是移植到了MSP432平台
 * 仅作为学习使用
 * 在此也感谢正点原子
 *********************************************/

#include "delay.h"

static uint8_t fac_us = 0;	//us延时倍乘数

//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
	fac_us = CS_getMCLK() / 1000000;		 //系统时钟
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //内部时钟源
}


//延时nus
//nus为要延时的us数.
//注意nus的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nus<=0xffffff*1000000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对48M条件下,nus<=349525 (349ms)
void delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->LOAD = nus * fac_us;			  //时间加载
	SysTick->VAL = 0x00;					  //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); //等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		//关闭计数器
	SysTick->VAL = 0X00;							//清空计数器
}
//延时nms
void delay_ms(uint32_t nms)
{
	while(nms)
	{
		delay_us(1000);
		--nms;
	}
}
