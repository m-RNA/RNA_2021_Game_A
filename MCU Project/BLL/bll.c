#include "bll.h"
#include "bsp.h"
#include "log.h"
#include "config.h"
#include "stdio.h"
#include "math.h"
#include "adc.h"
#include "oled_interface.h"
#include "arm_math.h"
#include "arm_const_structs.h"

void Signal_Sample_Init(void)
{
    // BSP_Sample_ADC_with_DMA_Init(Signal_ADC_Data, ADC_SAMPLING_NUM); // 第11讲 ADC 第12讲 DMA
    BSP_Sample_Timer_Init();
}

void Signal_F0_Measure(u32 *Captured_Value)
{
    log_debug("Signal F0 Measuring...\r\n");

    *Captured_Value = BSP_Get_Signal_CCR();

    log_debug("F0 Captured Value:%u\r\n", *Captured_Value);
    log_debug("F0(Captured):%ukHz\r\n", TimerSourerFreq / 1000 / (*Captured_Value));
}

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Signal_Fs_CCR = 0;
    log_debug("Signal Fs Adjusting...\r\n");

    Signal_Fs_CCR = Captured_Value / SignalSampleFreq_Multiple;
    if (Captured_Value <= SignalSamplePeriod_MIN)
    {
        Signal_Fs_CCR += Captured_Value;
        log_debug("Fs CCR: %u\r\n", Signal_Fs_CCR);
        log_debug("Actual Fs: %ukHz\r\n", TimerSourerFreq / 1000 / Signal_Fs_CCR);
        log_debug("Equivalent Fs: %ukHz\r\n", SignalSampleFreq_Multiple * TimerSourerFreq / 1000 / Captured_Value);
    }
    else
    {
        log_debug("Fs CCR: %u\r\n", Signal_Fs_CCR);
        log_debug("Fs: %ukHz\r\n", TimerSourerFreq / 1000 / Signal_Fs_CCR);
    }
    BSP_Set_Fs_CCR(Signal_Fs_CCR);
}

void SignalSample_Start(u16 *Data)
{
    log_debug("Signal Sampling...\r\n");
    BSP_ADC_DMA_Start(Data, ADC_SAMPLING_NUM);
    log_debug("Signal Sample Completed!\r\n\r\n");
}

static float fft_inputbuf[ADC_SAMPLING_NUM * 2];
void SignalSample_FFT_to_Am(u16 *SampleData, float *Output)
{
    u16 i;
    log_debug("FFT Calculating Amplitudes...\r\n");

    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[0 + (i << 1)] = SampleData[i]; // 实部为ADC
        fft_inputbuf[1 + (i << 1)] = 0;             // 虚部为0
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    arm_cmplx_mag_f32(fft_inputbuf, Output, ADC_SAMPLING_NUM);  //把运算结果复数求模得幅值

    log_debug("FFT Calculate Amplitudes Completed!\r\n");
}

/* 在一定范围内找出最大值位置 */
u16 FloatMaxIndex_WithinRange(float Data[], u16 Left, u16 Right) // 最优算法应为分治法
{
    u16 i, MaxIndex;
    MaxIndex = Left;
    for (i = Left; i <= Right; ++i)
    {
        if (Data[MaxIndex] < Data[i])
        {
            MaxIndex = i;
        }
    }
    return MaxIndex;
}

#ifdef Simulation
#define true_T Simulation_CCR[Simulation_Times_Index]
#endif

#define FFT_To_Am_IndexErrorRange 4
#define Fs (SignalSampleFreq_Multiple * TimerSourerFreq / true_T)
#define FFT_Freq_Calculate(Index) (Index * Fs / ADC_SAMPLING_NUM)
void NormalizedAm_And_CalculateTHD(float *Am_Data, float *NormAm, float *THD)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float sum = 0.0f;

    /* 找出基波位置 */
    Fx_Index[0] = FloatMaxIndex_WithinRange(Am_Data, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));
    log_debug("F0: %ukHz\r\n", FFT_Freq_Calculate(Fx_Index[0]) / 1000);

    for (i = 1; i < 5; ++i)
    {
        /* 找出谐波位置 */
        Fx_Index[i] = FloatMaxIndex_WithinRange(
            Am_Data,
            Fx_Index[0] * (i + 1) - (FFT_To_Am_IndexErrorRange >> 1),
            Fx_Index[0] * (i + 1) + (FFT_To_Am_IndexErrorRange >> 1)); // 优化过的算法 更加准确
        log_debug("F%u: %ukHz\r\n", (i + 1), FFT_Freq_Calculate(Fx_Index[i]) / 1000);

        /* 计算归一化幅值 */
        NormAm[i - 1] = floor(Am_Data[Fx_Index[i]] / Am_Data[Fx_Index[0]] * 100.0f) / 100.0f; // 向下取整floor() 误差更小
    }
    log_debug("Normalized Am: 1.000, %0.3f, %0.3f, %0.3f, %0.3f\r\n", NormAm[0], NormAm[1], NormAm[2], NormAm[3]); // 归一化幅值

    /* THDx计算 */
    for (i = 1; i < 5; ++i)
    {
        sum += Am_Data[Fx_Index[i]] * Am_Data[Fx_Index[i]];
    }
    *THD = ceil(sqrt(sum) / Am_Data[Fx_Index[0]] * 10000) / 100.0f; // 向上取整ceil()
    log_debug("THDx: %.3f%%\r\n\r\n", *THD);
}

// 找出最小值位置
u16 Compare_Min(float Mag[], u16 len)
{
    u16 i, Fn_Num;
    Fn_Num = 0;
    Mag[Fn_Num] = Mag[0];
    for (i = 1; i < len; i++)
    {
        if (Mag[Fn_Num] > Mag[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}

static float OriginalWaveDate[OLED_X_MAX];
void Signal_Synthesizer(u16 *Output, u16 Length, u16 Magnification, float *NormAm, u8 Precision)
{
    u16 i, j;
    u16 MinIndex;
    
    for (i = 0; i < Length; ++i)
    {
        OriginalWaveDate[i] = arm_sin_f32(PI * i / ((float)(Length >> 1)));
        for (j = 0; j < Precision - 1; ++j) // 各次谐波叠加
        {
            OriginalWaveDate[i] += arm_sin_f32(PI * i * (j + 2) / ((float)(Length >> 1))) * NormAm[j];
        }
    }

    // 找出最小的小数的位置
    MinIndex = Compare_Min(OriginalWaveDate, Length);
    for (i = 0; i < Length; ++i)
    {
        // 将小数全转为以0为起点的正数，再乘以 Magnification 变为整数
        Output[i] = Magnification * (OriginalWaveDate[i] - OriginalWaveDate[MinIndex]); // 将小数全转为为正数
    }
}

void Transform_NormalizedAm_To_WaveformData(float *NormAm, u16 *WaveformData)
{
    log_debug("Transforming Normalized Am To Waveform Data...\r\n");

    Signal_Synthesizer(WaveformData, OLED_X_MAX, 100, // 这个100是随便定的，不要太大就好了，目的是把小数转换为整数；OLED显示函数会进一步处理范围
                       NormAm, 5);

    log_debug("Transforming Completed!\r\n");
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
