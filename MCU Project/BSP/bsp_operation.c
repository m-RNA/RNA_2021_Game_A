#include "bsp_operation.h"
#include "bsp_it.h"
#include "log.h"
#include "stdlib.h"

/********************************************************************************************/
/*********************************   操作类函数  ********************************************/
void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
#if Simulation
#if 1 // 两种仿真输入信号生成方式选择（选一个就好）
    Simulate_Signal_Synthesizer(Data, Num);
#else
    Simulate_Signal_WaveformData(Data);
#endif
#else
#ifdef __MSP432P401R__
    MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)Data, Num);
    MAP_DMA_enableChannel(7); // 使能7通道（ADC）

    DMA_Transmit_Completed_Flag = 0;      // 传输完成标志位清零
    BSP_Timer_Start(Signal_Sample_Timer); // 开始计数 触发ADC定时采样
    while (!DMA_Transmit_Completed_Flag)  // 等待传输完成
        ;
#else
    DMA_Transmit_Completed_Flag = 0;      // 传输完成标志位清零
    BSP_Timer_Start(Signal_Sample_Timer); // 开始计数 触发ADC定时采样
    while (!DMA_Transmit_Completed_Flag)  // 等待传输完成
        ;
#endif
#endif
}

u32 BSP_Get_Signal_CCR(void)
{
#if Simulation
    if (Simulation_CCR_Data[Simulation_Times_Index] != Simulation_CCR)
        log_debug("Warning: Simulation_CCR Spilling!!!\r\n");
    return Simulation_CCR;
#else
    delay_ms(19); // 信号捕获最多时长也就 1.4ms * 6 = 8.2ms
    return BSP_Signal_Capture_Value;
#endif
}

void BSP_Set_Fs_ARR(u32 Fs_ARR)
{
#if Simulation
    Simulation_Set_Fs_ARR(Fs_ARR);
#else
#ifdef __MSP432P401R__
    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, Fs_ARR); // 调整fs
#else
    __HAL_TIM_SET_AUTORELOAD(SIGNAL_SAMPLE_TIMER, Fs_ARR);
#endif
#endif
}

static void BSP_Fs_Timer_Start(void)
{
#ifdef __MSP432P401R__
    MAP_Timer_A_startCounter(SIGNAL_SAMPLE_TIMER, TIMER_A_UP_MODE); //开启计数
#else
    HAL_TIM_Base_Start(SIGNAL_SAMPLE_TIMER);
#endif
}

static void BSP_Cap_Timer_Start(void)
{
#ifdef __MSP432P401R__
    MAP_Timer_A_startCounter(SIGNAL_CAPTURE_TIMER, TIMER_A_CONTINUOUS_MODE);
#else
    HAL_TIM_IC_Start_IT(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_CHANNEL);
#endif
}

static void BSP_Fs_Timer_Stop(void)
{
#ifdef __MSP432P401R__
    MAP_Timer_A_stopTimer(SIGNAL_SAMPLE_TIMER);
#else
    HAL_TIM_Base_Stop(SIGNAL_SAMPLE_TIMER);
#endif
}

static void BSP_Cap_Timer_Stop(void)
{
#ifdef __MSP432P401R__
    MAP_Timer_A_stopTimer(SIGNAL_CAPTURE_TIMER);
    MAP_Timer_A_clearTimer(SIGNAL_CAPTURE_TIMER); //清空定时器 重新从0计数
    // MAP_Timer_A_clearInterruptFlag(SIGNAL_CAPTURE_TIMER);//清除定时器溢出中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER); //清除 CCR1 更新中断标志位
#else
    HAL_TIM_IC_Stop_IT(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_CHANNEL);
#endif
}

void BSP_Timer_Start(BSP_Timer Timer)
{
    if (Timer == Signal_Sample_Timer)
    {
        BSP_Fs_Timer_Start();
    }
    else if (Timer == Signal_Capture_Timer)
    {
        BSP_Cap_Timer_Start();
    }
}

void BSP_Timer_Stop(BSP_Timer Timer)
{
    if (Timer == Signal_Sample_Timer)
    {
        BSP_Fs_Timer_Stop();
    }
    else if (Timer == Signal_Capture_Timer)
    {
        BSP_Cap_Timer_Stop();
    }
}

void BSP_Bluetooth_SendByte(u8 Data)
{
#if defined __MSP432P401R__
    MAP_UART_transmitData(BLUETOOTH_UART, Data);
#elif defined USE_HAL_DRIVER
    HAL_UART_Transmit(BLUETOOTH_UART, &Data, 1, 50);
#else
#error Please Transplant Function: BSP_Bluetooth_SendByte();
#endif
}

#ifdef __MSP432P401R__
void NVIC_Init(void)
{
}

void Delay_Init(void)
{
    delay_init();
}

void HAL_Init(void)
{
    NVIC_Init();
}

#endif
