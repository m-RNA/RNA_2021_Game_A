#include "my_math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "oled_interface.h"

static float WaveDateBuf[OLED_X_MAX];
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

void CalculateAmplitude_By_FFT(u16 *SampleData, float *Am_Data)
{
    u16 i;
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[0 + (i << 1)] = SampleData[i]; // 实部为数据
        fft_inputbuf[1 + (i << 1)] = 0;             // 虚部为0
    }
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    arm_cmplx_mag_f32(fft_inputbuf, Am_Data, ADC_SAMPLING_NUM); //把运算结果复数求模得幅值
}
