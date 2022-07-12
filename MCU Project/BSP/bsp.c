#include "config.h"
#include "bsp.h"
#include "log.h"
#include "stdlib.h"
#ifdef Simulation
#include "simulation.h"
#endif

/********************************************************************************************/
/******************************   初始化类函数  *********************************************/

void BSP_LED_KEY_BEEP_Init(void)
{
#ifdef __MSP432P401R__
    LED_Init(); // LED
#else
    MX_GPIO_Init();
#endif
}

void BSP_OLEDInterface_Init(void)
{
    InitGraph(); 
}

void BSP_Uart_PC_Init(void)
{
#ifdef __MSP432P401R__
    uart_init(1382400); // 第7讲 串口配置（调试）
#else
    MX_USART1_UART_Init(); // 第7讲 串口配置（调试）
#endif
    log_debug("Uart_PC Init Completed!\r\n");
}

void BSP_Uart_Bluetooth_Init(void)
{
#ifdef __MSP432P401R__
    usart3_init(9600);
#else
    MX_USART2_UART_Init();
#endif
    log_debug("config BSP_Uart_Bluetooth...\r\n");
}

void BSP_Sample_Timer_Init(void)
{
    log_debug("config BSP_Sample_Timer_Init...\r\n");
#ifdef __MSP432P401R__
    TimA0_Int_Init(60, 1); // 第8讲 定时器配置 （ADC触发时钟源 fs）
    TimA2_Cap_Init();      // 第8讲 定时器捕获 （过零比较器采频率）
#else
    MX_TIM3_Init(); // 第8讲 定时器配置 （ADC触发时钟源 fs）
    MX_TIM2_Init(); // 第8讲 定时器捕获 （过零比较器采频率）

    HAL_TIM_IC_Start_IT(SIGNAL_SAMPLE_TIMER, SIGNAL_SAMPLE_TIMER_CHANNEL);
    HAL_TIM_Base_Start(SIGNAL_SAMPLE_TIMER);
#endif
}

void BSP_Sample_ADC_with_DMA_Init(u16 *Addr, u16 Length)
{
#ifdef __MSP432P401R__
    adc_dma_init(Addr, Length); // 第12讲 DMA
    ADC_Config();               // 第11讲 ADC
#else
    MX_DMA_Init();
    MX_ADC1_Init();
#endif
    log_debug("config BSP_Sample_ADC_with_DMA_Init...\r\n");
}

/********************************************************************************************/
/***********************************   中断函数  ********************************************/

vu8 SignalCaptureTimerState = 0; // 捕获状态
vu16 true_T = 240;               // 真正的测量周期
#ifdef __MSP432P401R__
vu16 capTim[5];

uint16_t dt_1;
uint16_t dt_2;

// 过零比较器的误差
#define NOISE (1 << 5)

void TA2_N_IRQHandler(void)
{
    static uint8_t i = 0;
    // 清除 CCR1 更新中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
    if (!SignalCaptureTimerState) // 未捕获成功
    {
        if (i == 0)
        {
            MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);
        }
        else
        {
            capTim[i - 1] = MAP_Timer_A_getCaptureCompareCount(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
            if (i > 6)
            {
                // clear CCR1 and stop Timer
                MAP_Timer_A_stopTimer(CAP_TIMA_SELECTION);
                MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);
                i = 0;
                SignalCaptureTimerState = 1;
                dt_1 = capTim[3] - capTim[1];
                dt_2 = capTim[4] - capTim[2];
                if (dt_1 <= (dt_2 + NOISE) && dt_1 >= (dt_2 - NOISE))
                {
                    true_T = dt_1;
                    // printf("一般\r\t");
                }
                else
                {
                    true_T = capTim[4] - capTim[0];
                    // printf("想多\r\t");
                }
                // printf("t4:%d\r\t", capTim[4]);
                return;
            }
        }
        ++i;
    }
}
#else
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == *SIGNAL_SAMPLE_TIMER.Instance)
    {
        if (htim->Channel == SIGNAL_SAMPLE_TIMER_ACTIVE_CHANNEL)
        {
            //※总PWM周期
            true_T = HAL_TIM_ReadCapturedValue(htim, SIGNAL_SAMPLE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
            SignalCaptureTimerState = 1;
        }
    }
}
#endif

/********************************************************************************************/
/*********************************   操作类函数  ********************************************/

u32 BSP_Get_Signal_CCR(void)
{
#ifdef Simulation
    return Simulation_CCR[Simulation_Times_Index];
#else

#ifdef __MSP432P401R__
    
#else
    
#endif
    
    delay_ms(19); // 信号捕获最多时长也就 1.4ms * 6 = 8.2ms
    // while(SignalCaptureTimerState == 0); // 阻塞 再次确定
    // SignalCaptureTimerState = 0;
    return true_T;
#endif
}

void BSP_Set_Fs_CCR(u32 Fs_CCR)
{
#ifdef __MSP432P401R__
    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, Fs_CCR); // 调整fs
#else
    __HAL_TIM_SET_AUTORELOAD(SIGNAL_SAMPLE_TIMER, Fs_CCR);
#endif
}

void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
#ifdef Simulation
#if 1 // 两种仿真输入信号生成方式选择（选一个就好）
    Simulate_Signal_Synthesizer(Data);
#else
    Simulate_Signal_WaveformData(Data); 
#endif
    
#else
#ifdef __MSP432P401R__
    MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)Data, Num);
    MAP_DMA_enableChannel(7); // 使能7通道（ADC）

    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); // 开始计数 触发ADC定时采样
    recv_done_flag = 0;                                       // 传输完成标志位清零
    while (!recv_done_flag)                                   // 等待传输完成
        ;
#else
    HAL_ADC_Start_DMA(SIGNAL_SAMPLE_ADC, (u32 *)Data, Num);
    // ....

//    recv_done_flag = 0;     // 传输完成标志位清零
//    while (!recv_done_flag) // 等待传输完成
//        ;
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
