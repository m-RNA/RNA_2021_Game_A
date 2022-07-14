#ifndef __BSP_INIT_H__
#define __BSP_INIT_H__
#include "config.h"

void BSP_GPIO_Init(void);
void BSP_OLEDInterface_Init(void);

void BSP_Uart_PC_Init(void);
void BSP_Uart_Bluetooth_Init(void);

void BSP_Sample_Timer_Init(void);
void BSP_Sample_ADC_with_DMA_Init(u16 *Addr, u16 Length);

#endif
