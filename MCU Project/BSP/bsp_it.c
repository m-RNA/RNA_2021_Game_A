#include "bsp_it.h"
#include "bsp_operation.h"

static vu32 Internal_Cap_Register = 0; // 捕获值
static vu8 CapTimer_SyncState = 0;     // 捕获信号同步状态

/********************************************************************************************/
/***********************************   中断函数  ********************************************/

vu8 DMA_Transmit_Completed_Flag = 0; // DMA搬运完成标志
vu16 BSP_Signal_Capture_Value = 240; // 平均捕获值

#ifdef __MSP432P401R__
void TA2_N_IRQHandler(void)
{
    // 清除 CCR1 更新中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);

    CapTimer_SyncState++;
    if (CapTimer_SyncState == 1) // 第一次捕获值位于信号同步 不使用该数据
    {
        MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER); // 将该值读走
        MAP_Timer_A_clearTimer(SIGNAL_CAPTURE_TIMER);                                            //清空定时器 重新从0计数
        Internal_Cap_Register = 0;
        return;
    }
    if (CapTimer_SyncState <= CAP_TIMES) //
    {
        Internal_Cap_Register += MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
        return;
    }
    Internal_Cap_Register += MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
    BSP_Timer_Stop(Signal_Capture_Timer);

    CapTimer_SyncState = 0;
    BSP_Signal_Capture_Value = Internal_Cap_Register / CAP_TIMES;
}

void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_clearInterruptFlag(7);
    MAP_DMA_disableChannel(7); // dma will auto disable channel if complete
    // MAP_ADC14_clearInterruptFlag(ADC_INT0);

    BSP_Timer_Stop(Signal_Sample_Timer);
    DMA_Transmit_Completed_Flag = 1;
}
#else
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == *SIGNAL_CAPTURE_TIMER.Instance)
    {
        if (htim->Channel == SIGNAL_CAPTURE_TIMER_ACTIVE_CHANNEL)
        {
            CapTimer_SyncState++;
            if (CapTimer_SyncState == 1) // 第一次捕获值位于信号同步 不使用该数据
            {
                HAL_TIM_ReadCapturedValue(htim, SIGNAL_CAPTURE_TIMER_CHANNEL); // 将该值读走
                Internal_Cap_Register = 0;
                return;
            }
            if (CapTimer_SyncState <= CAP_TIMES) //
            {
                Internal_Cap_Register += HAL_TIM_ReadCapturedValue(htim, SIGNAL_CAPTURE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
                return;
            }
            Internal_Cap_Register += HAL_TIM_ReadCapturedValue(htim, SIGNAL_CAPTURE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
            BSP_Timer_Stop(Signal_Capture_Timer);

            BSP_Signal_Capture_Value = Internal_Cap_Register / CAP_TIMES; //※是TIM_CHANNEL_1 要记得加1
        }
    }
}

void DMA_ADC_Transmit_Complete_Callback(DMA_HandleTypeDef *_hdma)
{
    if (_hdma->Instance == *DMA_ADC.Instance)
    {
        BSP_Timer_Stop(Signal_Sample_Timer);
        DMA_Transmit_Completed_Flag = 1;
    }
}

#endif
