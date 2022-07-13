#include "my_math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "oled_interface.h"
#include "log.h"

#define FFT_To_Am_IndexErrorRange 4

static float WaveDateBuf[Signal_Synthesizer_Wave_Length_MAX];
static float fft_inputbuf[ADC_SAMPLING_NUM * 2];

/*
 * 提示：同时找出最大值和最小值
 * 最优算法应为分治法 时间复杂度 O(3/2n)
 */

/* 找出最小值位置 */
u16 Min_Float(float Mag[], u16 len)
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

/* 找出最大值位置 */
u16 Max_Unsigned(u16 Mag[], u16 len)
{
    u16 i, Fn_Num;
    Fn_Num = 0;
    Mag[Fn_Num] = Mag[0];
    for (i = 1; i < len; i++)
    {
        if (Mag[Fn_Num] < Mag[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}

/* 在一定范围内找出最大值位置 */
u16 Max_Float_WithinRange(float Data[], u16 Left, u16 Right)
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

/**
 * @brief  信号合成器
 * @param[out] Output        波形数据输出指针
 * @param[in]  Magnification 放大倍率
 * @param[in]  NormAm        归一化幅值
 * @param[in]  Precision     最高几次谐波分量
 */
void Signal_Synthesizer(u16 *Output, u16 Length, u16 Magnification, float *NormAm, u8 Precision)
{
    u16 i, j;
    u16 MinIndex;

    for (i = 0; i < Length; ++i)
    {
        WaveDateBuf[i] = arm_sin_f32(PI * i / ((float)(Length >> 1)));
        for (j = 0; j < Precision - 1; ++j) // 各次谐波叠加
        {
            if (NormAm[j] == 0.0f)
                continue;
            WaveDateBuf[i] += arm_sin_f32(PI * i * (j + 2) / ((float)(Length >> 1))) * NormAm[j];
        }
    }

    // 找出最小的小数的位置
    MinIndex = Min_Float(WaveDateBuf, Length);
    for (i = 0; i < Length; ++i)
    {
        // 将小数全转为以0为起点的正数，再乘以 Magnification 变为整数
        Output[i] = Magnification * (WaveDateBuf[i] - WaveDateBuf[MinIndex]);
    }
}

/* 通过FFT 计算各个频率分量幅值 */
void CalculateAmplitude_By_FFT(float *Am_Data, u16 *SampleData)
{
    u16 i;
    log_debug("Calculating Amplitude...\r\n");

    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[0 + (i << 1)] = SampleData[i]; // 实部为数据
        fft_inputbuf[1 + (i << 1)] = 0;             // 虚部为0
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    arm_cmplx_mag_f32(fft_inputbuf, Am_Data, ADC_SAMPLING_NUM); //把运算结果复数求模得幅值

    log_debug("Calculating Amplitude Completed!\r\n");
}

/*  计算各个频率分量幅值 */
void NormalizedAm_And_CalculateTHD(float *NormAm, float *THD, float *Am_Data)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float Square_Sum = 0.0f;

    /* 找出基波位置 */
    Fx_Index[0] = Max_Float_WithinRange(Am_Data, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));
    for (i = 1; i < 5; ++i)
    {
        /* 找出谐波位置 */
        Fx_Index[i] = Max_Float_WithinRange(
            Am_Data,
            Fx_Index[0] * (i + 1) - (FFT_To_Am_IndexErrorRange >> 1),
            Fx_Index[0] * (i + 1) + (FFT_To_Am_IndexErrorRange >> 1)); // 优化过的算法 更加准确

        /* 计算归一化幅值 */
        NormAm[i - 1] = floor(Am_Data[Fx_Index[i]] / Am_Data[Fx_Index[0]] * 100.0f) / 100.0f; // 向下取整floor() 误差更小

        /* THDx部分计算 */
        Square_Sum += Am_Data[Fx_Index[i]] * Am_Data[Fx_Index[i]]; // 平方和
    }
    *THD = ceil(sqrtf(Square_Sum) / Am_Data[Fx_Index[0]] * 10000) / 100.0f; // 向上取整ceil()

    log_Fn_NAm_THD_data(Fx_Index, NormAm, *THD);
}

/* 将归一化幅值转化为波形数据 */
void Transform_NormalizedAm_To_WaveformData(float *NormAm, u16 *WaveformData)
{
    log_debug("Transforming Normalized Am To Waveform Data...\r\n");

    Signal_Synthesizer(WaveformData, OLED_X_MAX, 256, // 这个256是随便定的，目的是把小数转换为整数；OLED显示函数会进一步处理范围
                       NormAm, 5);

    log_debug("Transforming Completed!\r\n");
}
