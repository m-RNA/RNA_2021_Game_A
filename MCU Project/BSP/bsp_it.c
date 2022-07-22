#include "bsp_it.h"
#include "bsp_operation.h"

vu32 Cap_Val[CAP_TIMES] = {0};          // 捕获值
static vu8 CapTimer_SyncState = 0;      // 捕获信号同步状态
static vu8 CapTimer_Spilling_Times = 0; // 捕获溢出次数

/********************************************************************************************/
/***********************************   中断函数  ********************************************/

vu8 DMA_Transmit_Completed_Flag = 0; // DMA搬运完成标志
vu32 BSP_Signal_Avrg_Cap_Val = 0;    // 平均捕获值

#ifdef __MSP432P401R__

void TA2_N_IRQHandler(void)
{
#define PASS_TIMES 1
    if (MAP_Timer_A_getCaptureCompareEnabledInterruptStatus(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER)) //捕获中断
    {
        // 清除 CCR1 更新中断标志位
        MAP_Timer_A_clearCaptureCompareInterrupt(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);

        if (CapTimer_SyncState == 0) // 第一次捕获值位于信号同步 不使用该数据
        {
            MAP_Timer_A_clearTimer(SIGNAL_CAPTURE_TIMER); // 清空定时器 重新从0计数
            CapTimer_SyncState = 1;
            return;
        }

        if (CapTimer_SyncState < CAP_TIMES) //
        {
            Cap_Val[CapTimer_SyncState - PASS_TIMES] = (CapTimer_Spilling_Times * 0xFFFF) +
                                                       MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
            CapTimer_SyncState++;
            return;
        }
        Cap_Val[CapTimer_SyncState - PASS_TIMES] = (CapTimer_Spilling_Times * 0xFFFF) +
                                                   MAP_Timer_A_getCaptureCompareCount(SIGNAL_CAPTURE_TIMER, SIGNAL_CAPTURE_TIMER_REGISTER);
        BSP_Timer_Stop(Signal_Capture_Timer);

        CapTimer_SyncState = 0;
        CapTimer_Spilling_Times = 0;
        BSP_Signal_Avrg_Cap_Val = Cap_Val[CAP_TIMES - 1] / CAP_TIMES;
    }

    if (MAP_Timer_A_getEnabledInterruptStatus(SIGNAL_CAPTURE_TIMER)) //溢出中断
    {
        MAP_Timer_A_clearInterruptFlag(SIGNAL_CAPTURE_TIMER); //清除定时器溢出中断标志位

        /* ★ 软件复位COV ★ */
        /* 这里UP忘记讲了，如果在未清除中断位值时，来了一次中断，COV会置位，需要软件复位，这里没有官方库函数。具体可以参考技术手册(slau356h.pdf) P790 */
        BITBAND_PERI(TIMER_A_CMSIS(SIGNAL_CAPTURE_TIMER)->CCTL[(SIGNAL_CAPTURE_TIMER_REGISTER >> 1) - 1], TIMER_A_CCTLN_COV_OFS) = 0;

        ++CapTimer_Spilling_Times; // 溢出次数加1
    }
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
                Cap_Sum = 0;
                return;
            }
            if (CapTimer_SyncState <= CAP_TIMES) //
            {
                Cap_Sum += HAL_TIM_ReadCapturedValue(htim, SIGNAL_CAPTURE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
                return;
            }
            Cap_Sum += HAL_TIM_ReadCapturedValue(htim, SIGNAL_CAPTURE_TIMER_CHANNEL) + 1; //※是TIM_CHANNEL_1 要记得加1
            BSP_Timer_Stop(Signal_Capture_Timer);

            BSP_Signal_Capture_Value = Cap_Sum / CAP_TIMES; //※是TIM_CHANNEL_1 要记得加1
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
