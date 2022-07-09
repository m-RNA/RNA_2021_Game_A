#ifndef __BSP_H__
#define __BSP_H__

#include "main.h"

void BSP_Sample_ADC_with_DMA_Init(void);

void BSP_Sample_Timer_Init(void);

void BSP_Uart_PC(void);
void BSP_Uart_Bluetooth(void);
void BSP_LED_Init(void);
void BSP_BEEP_Init(void);
void BSP_KEY_Init(void);
void BSP_Init(void);

void BSP_Set_Fs_CCR(u32 CCR);
u32 BSP_Get_Signal_CCR(void);
void BSP_ADC_DMA_Start(u16 *Data, u16 Num);

#endif
