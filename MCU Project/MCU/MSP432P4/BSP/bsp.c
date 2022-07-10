#include "bsp.h"
#include "log.h"
#include "stdlib.h"
#include "math.h"
#include "timA.h"
#include "delay.h"
#include "adc.h"

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

void BSP_Uart_Bluetooth(void)
{
    log_debug("config BSP_Uart_Bluetooth...\r\n");
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

void BSP_Init(void)
{
    BSP_Uart_PC();
    BSP_Uart_Bluetooth();
    BSP_LED_Init();
    BSP_BEEP_Init();
    BSP_KEY_Init();
}

void BSP_Set_Fs_CCR(u32 Fs_CCR)
{
    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, Fs_CCR); // 调整fs
}

u32 BSP_Get_Signal_CCR(void)
{
    while(SignalCaptureTimerState == 0); // 阻塞
    
    SignalCaptureTimerState = 0; // 捕获完成标志位清零
    MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);                            // 捕获定时器清零
    MAP_Timer_A_startCounter(CAP_TIMA_SELECTION, TIMER_A_CONTINUOUS_MODE); // 开始测量f

    return true_T;

    //return (rand() & 0xFF);
}


#define Y_RANGE_OF_WAVE 54
#define Y_UP_MOVE 64

void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
    MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)Data, Num);
    MAP_DMA_enableChannel(7); // 使能7通道（ADC）

    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); // 开始计数 触发ADC定时采样

    recv_done_flag = 0;     // 传输完成标志位清零
    while (!recv_done_flag) // 等待传输完成
        ;
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



//void HAL_Init(void)
//{
//    NVIC_Init();
//    Clock_Init();
//    Delay_Init();
//}


