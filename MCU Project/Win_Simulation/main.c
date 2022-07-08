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

// #ifdef DEBUG
// #define log_debug(fmt, ...) __log_print(fmt, );
// #else
// #define log_debug(fmt, ...) ((void)0);
// #endif

void log_debug(char *fmt, ...)
{
#ifdef DEBUG
    va_list arg;
    va_start(arg, fmt);
    char buf[1 + vsnprintf(NULL, 0, fmt, arg)];
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);
    printf("%s\n", buf);
#endif
}

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

void BSP_Sample_ADC_with_DMA_Init(void)
{
    log_debug("config BSP_Sample_ADC_with_DMA_Init...\r\n");
}

void BSP_Sample_Timer_Init(void)
{
    log_debug("config BSP_Sample_Timer_Init...\r\n");
}

void BSP_Uart_PC(void)
{
    log_debug("config BSP_Uart_PC...\r\n");
}

void BSP_Uart_Blueteech(void)
{
    log_debug("config BSP_Uart_Blueteech...\r\n");
}

void BSP_LED_Init(void)
{
    log_debug("config BSP_LED_Init...\r\n");
}

void BSP_BEEP_Init(void)
{
    log_debug("config BSP_BEEP_Init...\r\n");
}

void BSP_KEY_Init(void)
{
    log_debug("config BSP_KEY_Init...\r\n");
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

void BSP_Init(void)
{
    BSP_Uart_PC();
    BSP_Uart_Blueteech();
    BSP_LED_Init();
    BSP_BEEP_Init();
    BSP_KEY_Init();
}

void System_Init(void)
{
    HAL_Init();
    BSP_Init();
    BLL_Init();

    Signal_Sample_Init();
}

void BSP_Set_Fs_CCR(u32 CCR)
{
    return;
}
u32 BSP_Get_Signal_CCR(void)
{
    return (rand() & 0xFF);
}

void Signal_F0_Measure(u32 *Captured_Value)
{
    *Captured_Value = BSP_Get_Signal_CCR();
    log_debug("F0_CCR:%u\r\n", *Captured_Value);
}

#define TimerSoureFreq 48000000
#define SignalSample_Point_Bit 4 //采样位数（采样点数 = 2^采样位数)
#define SignalSample_Freq_MAX 1000000
#define SignalSample_Period_MIN (TimerSoureFreq / Signal_Fs_MAX)

void Signal_Fs_Adjust_Auto(u32 Captured_Value)
{
    u32 Fs_CCR = 0;
    Fs_CCR = Captured_Value >> SignalSample_Point_Bit;

    if (Captured_Value <= SignalSample_Period_MIN)
    {
        Fs_CCR += Captured_Value;
        printf("Oversampling!\r\n");
    }
    BSP_Set_Fs_CCR(Fs_CCR);
    log_debug("Fs_CCR:%u\r\n", Fs_CCR);
}

void SignalSample_Start(u16 *Data, u16 Num)
{
    BSP_ADC_DMA_Start(Data, Num);
}
// Flow lock
int main(void)
{
    u32 Signal_Captured_Value;
    vu8 i = 10;
    System_Init();
    while (--i)
    {
        Signal_F0_Measure(&Signal_Captured_Value);
        Signal_Fs_Adjust_Auto(Signal_Captured_Value);
        SignalSample_Start();
        // SignalSample_FFT();
    }
    getchar();
}
