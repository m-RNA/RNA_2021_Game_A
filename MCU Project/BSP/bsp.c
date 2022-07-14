#include "config.h"
#include "bsp.h"
#include "log.h"
#include "stdlib.h"

/********************************************************************************************/
/******************************   初始化类函数  *********************************************/

void BSP_GPIO_Init(void)
{
#ifdef __MSP432P401R__
    LED_Init(); // LED
#else
    MX_GPIO_Init();
#endif
}

void BSP_OLEDInterface_Init(void)
{
    log_debug("OLED Interface Init...\r\n");
    InitGraph();
}

void BSP_Uart_PC_Init(void)
{
#ifdef __MSP432P401R__
    uart_init(1382400); // 第7讲 串口配置（调试）
#else
    MX_USART1_UART_Init(); // 第7讲 串口配置（调试）
#endif
    log_debug("Uart PC Init Completed!\r\n");
}

void BSP_Uart_Bluetooth_Init(void)
{
    log_debug("Uart Bluetooth Init...\r\n");
#ifdef __MSP432P401R__
    usart3_init(9600);
#else
    MX_USART2_UART_Init();
#endif
}

void BSP_Sample_Timer_Init(void)
{
    log_debug("Sample Timer Init...\r\n");
#ifdef __MSP432P401R__
    TimA2_Cap_Init(TIMER_A_CLOCKSOURCE_DIVIDER_1);                           // 第8讲 定时器捕获 （过零比较器采频率）
    TimA0_Int_Init(TimerSourerFreq / 500000, TIMER_A_CLOCKSOURCE_DIVIDER_1); // 第8讲 定时器配置 （ADC触发时钟源 fs）
#else
    MX_TIM2_Init(); // 第8讲 定时器捕获 （过零比较器采频率）
    MX_TIM3_Init(); // 第8讲 定时器配置 （ADC触发时钟源 fs）

    HAL_TIM_IC_Start_IT(SIGNAL_SAMPLE_TIMER, SIGNAL_SAMPLE_TIMER_CHANNEL);
    HAL_TIM_Base_Start(SIGNAL_SAMPLE_TIMER);
#endif
}

void BSP_Sample_ADC_with_DMA_Init(u16 *Addr, u16 Length)
{
    log_debug("Sample ADC And DMA Init...\r\n");
#ifdef __MSP432P401R__
    adc_dma_init(Addr, Length); // 第12讲 DMA
    ADC_Config();               // 第11讲 ADC
#else
    MX_DMA_Init();
    MX_ADC1_Init();
#endif
}

/********************************************************************************************/
/***********************************   中断函数  ********************************************/

vu8 Synchronization_CaptureTimerState = 0; // 捕获信号同步状态
vu8 DMA_Transmit_Completed_Flag = 0;       // DMA搬运完成标志
vu16 true_T = 240;                         // 捕获值

#ifdef __MSP432P401R__
void TA2_N_IRQHandler(void)
{
    // 清除 CCR1 更新中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
    if (!Synchronization_CaptureTimerState) // 第一次捕获值位于信号同步 不使用该数据
    {
        Synchronization_CaptureTimerState = 1;
        MAP_Timer_A_getCaptureCompareCount(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
        return;
    }
    true_T = MAP_Timer_A_getCaptureCompareCount(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
}

void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_clearInterruptFlag(7);

    MAP_Timer_A_stopTimer(TIMER_A0_BASE);
    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    MAP_ADC14_clearInterruptFlag(ADC_INT0);

    DMA_Transmit_Completed_Flag = 1;

    // DMA_disableChannel(7);	// dma will auto disable channel if complete
}
#else
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == *SIGNAL_SAMPLE_TIMER.Instance)
    {
        if (htim->Channel == SIGNAL_SAMPLE_TIMER_ACTIVE_CHANNEL)
        {
            if (!Synchronization_CaptureTimerState) // 第一次捕获值位于信号同步 不使用该数据
            {
                Synchronization_CaptureTimerState = 1;
                HAL_TIM_ReadCapturedValue(htim, SIGNAL_SAMPLE_TIMER_CHANNEL);
                return;
            }
            true_T = HAL_TIM_ReadCapturedValue(htim, SIGNAL_SAMPLE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
        }
    }
}
#endif

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

    DMA_Transmit_Completed_Flag = 0;                          // 传输完成标志位清零
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); // 开始计数 触发ADC定时采样
    while (!DMA_Transmit_Completed_Flag)                      // 等待传输完成
        ;
#else
    HAL_ADC_Start_DMA(SIGNAL_SAMPLE_ADC, (u32 *)Data, Num);
    // ....

//    DMA_Transmit_Completed_Flag = 0;     // 传输完成标志位清零
//    while (!DMA_Transmit_Completed_Flag) // 等待传输完成
//        ;
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

#ifdef __MSP432P401R__

#else

#endif

    delay_ms(19); // 信号捕获最多时长也就 1.4ms * 6 = 8.2ms
    // while(Synchronization_CaptureTimerState == 0); // 阻塞 再次确定
    // Synchronization_CaptureTimerState = 0;
    return true_T;
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
