#ifndef __BSP_H__
#define __BSP_H__

#include "config.h"

extern vu16 true_T;

void BSP_GPIO_Init(void);
void BSP_OLEDInterface_Init(void);
void BSP_Uart_PC_Init(void);
void BSP_Uart_Bluetooth_Init(void);
void BSP_Sample_Timer_Init(void);
void BSP_Sample_ADC_with_DMA_Init(u16 *Addr, u16 Length);

u32 BSP_Get_Signal_CCR(void);
void BSP_Set_Fs_CCR(u32 Fs_CCR);
void BSP_ADC_DMA_Start(u16 *Data, u16 Num);
void BSP_Bluetooth_SendByte(u8 Data);

#endif
