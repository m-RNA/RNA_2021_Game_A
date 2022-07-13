#include "config.h"
#include "bll.h"
#include "bsp.h"
#include "log.h"

u8 OverSamplingFlag = 0;

void Signal_F0_Measure(u32 *Captured_Value)
{
    log_debug("Signal F0 Measuring...\r\n");

    *Captured_Value = BSP_Get_Signal_CCR();

    log_debug("F0 Captured Value:%u\r\n", *Captured_Value);
    log_debug("F0(Captured):%ukHz\r\n", TimerSourerFreq / 1000 / (*Captured_Value));
}

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Signal_Fs_ARR = 0;
    log_debug("Signal Fs Adjusting...\r\n");

    Signal_Fs_ARR = Captured_Value / SignalSampleFreq_Multiple;
    OverSamplingFlag = Captured_Value <= (SignalSamplePeriod_MIN);
    if (OverSamplingFlag)
    {
        Signal_Fs_ARR += Captured_Value;
    }
    BSP_Set_Fs_ARR(Signal_Fs_ARR);

    log_Fs_data(Captured_Value, Signal_Fs_ARR, OverSamplingFlag);
}

void SignalSample_Start(u16 *Data)
{
    log_debug("Signal Sampling...\r\n");

    BSP_ADC_DMA_Start(Data, ADC_SAMPLING_NUM);

    log_debug("Signal Sample Completed!\r\n\r\n");
}

void Bluetooth_SendDate_To_Phone(float *NormalizedAm, float THDx, u16 *WaveData)
{
    uint8_t i;
    log_debug("Bluetooth Sending Date To Phone...\r\n");

    /* 发送THD */
    BSP_Bluetooth_SendByte(((uint16_t)(THDx * 100)) >> 8);
    BSP_Bluetooth_SendByte(((uint16_t)(THDx * 100)) & 0xFF);

    /* 发送拟合值 */
    for (i = 0; i < 128; ++i)
    {
        BSP_Bluetooth_SendByte(WaveData[i] >> 8);
        BSP_Bluetooth_SendByte(WaveData[i] & 0xFF);
    }

    /* 发送归一化幅值 */
    for (i = 0; i < 4; ++i)
    {
        BSP_Bluetooth_SendByte(((uint16_t)(NormalizedAm[i + 1] * 100)) >> 8);
        BSP_Bluetooth_SendByte(((uint16_t)(NormalizedAm[i + 1] * 100)) & 0xFF);
    }
    log_debug("Bluetooth Sending Completed!\r\n");
}
