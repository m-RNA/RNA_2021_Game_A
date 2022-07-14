#include "bsp_it.h"
#include "log.h"
#include "stdlib.h"

/********************************************************************************************/
/***********************************   中断函数  ********************************************/

vu8 Synchronization_CaptureTimerState = 0; // 捕获信号同步状态
vu8 DMA_Transmit_Completed_Flag = 0;       // DMA搬运完成标志
vu16 BSP_Signal_Capture_Value = 240;       // 捕获值

#ifdef __MSP432P401R__
void TA2_N_IRQHandler(void)
{
    // 清除 CCR1 更新中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
    if (!Synchronization_CaptureTimerState) // 第一次捕获值位于信号同步 不使用该数据
    {
        Synchronization_CaptureTimerState = 1;
        MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
        return;
    }
    BSP_Signal_Capture_Value = MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
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
            BSP_Signal_Capture_Value = HAL_TIM_ReadCapturedValue(htim, SIGNAL_SAMPLE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
        }
    }
}
#endif
