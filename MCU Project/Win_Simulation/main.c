/*******************************************
// 2021年 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/11/11
*******************************************/

#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "log.h"
#include "bsp.h"

void NVIC_Init(void)
{
    log_debug("config NVIC...\r\n");
}

void Clock_Init(void)
{
    log_debug("config Clock_Init...\r\n");
}

void Delay_Init(void)
{
    log_debug("config Delay_Init...\r\n");
}

void BLL_Init(void)
{
    log_debug("config BLL_Init...\r\n");
}

void HAL_Init(void)
{
    NVIC_Init();
    Clock_Init();
    Delay_Init();
}

void Signal_Sample_Init(void)
{
    BSP_Sample_ADC_with_DMA_Init();
    BSP_Sample_Timer_Init();
}

void System_Init(void)
{
    HAL_Init();
    BSP_Init();
    BLL_Init();

    Signal_Sample_Init();
}

void Signal_F0_Measure(u32 *Captured_Value)
{
    *Captured_Value = BSP_Get_Signal_CCR();
    log_debug("F0_CCR:%u\r\n", *Captured_Value);
}

#define TimerSourerFreq 48000000
#define SignalSample_Freq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSample_Freq_MAX 1000000
#define SignalSample_Period_MIN (TimerSourerFreq / SignalSample_Freq_MAX)

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Fs_CCR = 0;
    Fs_CCR = Captured_Value / SignalSample_Freq_Multiple;

    if (Captured_Value <= SignalSample_Period_MIN)
    {
        Fs_CCR += Captured_Value;
        printf("Oversampling!\r\n");
    }
    BSP_Set_Fs_CCR(Fs_CCR);
    log_debug("Fs_CCR:%u\r\n", Fs_CCR);
}

void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
    return;
}

void SignalSample_Start(u16 *Data, u16 Num)
{
    BSP_ADC_DMA_Start(Data, Num);
}
// Flow lock

#define ADC_SAMPLING_NUM 1024
int main(void)
{
    vu8 i = 10;
    u32 Signal_Captured_Value;
    u16 Signal_ADC_Data[ADC_SAMPLING_NUM];

    System_Init();
    log_debug("初始化完成\r\n");
    while (--i)
    {
        Signal_F0_Measure(&Signal_Captured_Value);
        Signal_Fs_Adjust(Signal_Captured_Value);
        SignalSample_Start(Signal_ADC_Data, ADC_SAMPLING_NUM);
        // SignalSample_FFT();
    }
    getchar();
}
