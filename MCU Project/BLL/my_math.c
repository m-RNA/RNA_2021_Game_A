#include "my_math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "oled_interface.h"
#include "log.h"

#if (ADC_SAMPLING_NUM == 16u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len16
#elif (ADC_SAMPLING_NUM == 32u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len32
#elif (ADC_SAMPLING_NUM == 64u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len64
#elif (ADC_SAMPLING_NUM == 128u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len128
#elif (ADC_SAMPLING_NUM == 256u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len256
#elif (ADC_SAMPLING_NUM == 512u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len512
#elif (ADC_SAMPLING_NUM == 1024u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len1024
#elif (ADC_SAMPLING_NUM == 2048u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len2048
#elif (ADC_SAMPLING_NUM == 4096u)
#define ARM_FFT_USING_STRUCTURE arm_cfft_sR_f32_len4096
#endif

#define FFT_To_Am_IndexErrorRange 4

static float Synthetic_WaveBuf[Signal_Synthesizer_Wave_Length_MAX];
static float FFT_Input_Buf[ADC_SAMPLING_NUM * 2];

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
 * @param[out] Output    波形数据输出指针
 * @param[in]  Length    波形数据输出长度
 * @param[in]  F0_Vpp    基波幅值(mv)
 * @param[in]  NormAm    归一化幅值
 * @param[in]  Phase     相位
 * @param[in]  Precision 最高几次谐波分量
 */
void Signal_Synthesizer(u16 *Output, u16 Length, u16 F0_Vpp, float *NormAm, float *Phase, u8 Precision)
{
    u16 i, j;
    u16 MinIndex;

    for (i = 0; i < Length; ++i)
    {
        Synthetic_WaveBuf[i] = arm_sin_f32(PI * i / ((float)(Length >> 1)) + Phase[0]);
        for (j = 0; j < Precision - 1; ++j) // 各次谐波叠加
        {
            if (NormAm[j] == 0.0f)
                continue;
            Synthetic_WaveBuf[i] += arm_sin_f32(PI * i * (j + 2) / ((float)(Length >> 1)) + Phase[j]) * NormAm[j];
        }
    }

    // 找出最小的小数的位置
    MinIndex = Min_Float(Synthetic_WaveBuf, Length);
    for (i = 0; i < Length; ++i)
    {
        // 将小数全转为以0为起点的正数 再乘以 F0_Vpp 变为整数
        Output[i] = F0_Vpp * (Synthetic_WaveBuf[i] - Synthetic_WaveBuf[MinIndex]);
    }
}

/**
 * @brief  信号合成器
 * @param[out] Output    波形数据输出指针
 * @param[in]  Length    波形数据输出长度
 * @param[in]  Fx_Vpp    基波-谐波幅值(mv)
 * @param[in]  Phase     相位(弧度)
 * @param[in]  Precision 最高几次谐波分量
 */
void Signal_Synthesizer_Vpp(u16 *Output, u16 Length, u16 *Fx_Vpp, float *Phase, u8 Precision)
{
    u16 i, j;
    u16 MinIndex;

    for (i = 0; i < Length; ++i)
    {
        Synthetic_WaveBuf[i] = (Fx_Vpp[0] / 2.0f) * arm_sin_f32(PI * i / ((float)(Length >> 1)) + Phase[0]);
        for (j = 1; j < Precision; ++j) // 各次谐波叠加
        {
            if (Fx_Vpp[j] == 0)
                continue;
            Synthetic_WaveBuf[i] += (Fx_Vpp[j] / 2.0f) * arm_sin_f32(PI * i * (j + 1) / ((float)(Length >> 1)) + Phase[j]);
        }
    }
    // 找出最小的小数的位置
    MinIndex = Min_Float(Synthetic_WaveBuf, Length);
    for (i = 0; i < Length; ++i)
    {
        // 将小数全转为以0为起点的正数 再乘以 F0_Vpp 变为整数
        Output[i] = (Synthetic_WaveBuf[i] - Synthetic_WaveBuf[MinIndex]);
    }
}

/* 通过FFT 计算各个频率分量幅值 */
void CalculateAmplitude_By_FFT(float *Am_Pointer, u16 *SampleData_Pointer)
{
    u16 i;
    log_detail("Calculating Amplitude...\r\n");

    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        FFT_Input_Buf[0 + (i << 1)] = SampleData_Pointer[i]; // 实部为数据
        FFT_Input_Buf[1 + (i << 1)] = 0;                     // 虚部为0
    }
    arm_cfft_f32(&ARM_FFT_USING_STRUCTURE, FFT_Input_Buf, 0, 1);    // FFT计算
    arm_cmplx_mag_f32(FFT_Input_Buf, Am_Pointer, ADC_SAMPLING_NUM); //把运算结果复数求模得幅值

    log_detail("Calculating Amplitude Completed!\r\n");
}

/*  计算各个谐波分量幅值相位 */
void NormalizedAm_And_CalculateTHD(float *Phase_Pointer, float *NormAm_Pointer, u16 *Fx_Vpp_Pointer, float *THD_Pointer, float *Am_Data_Pointer)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float Square_Sum = 0.0f;

    /* 找出基波位置 */
    Fx_Index[0] = Max_Float_WithinRange(Am_Data_Pointer, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));
    Fx_Vpp_Pointer[0] = Am_Data_Pointer[Fx_Index[0]] * 4 / ADC_SAMPLING_NUM;
    Phase_Pointer[0] = atan2((FFT_Input_Buf[Fx_Index[0] << 1] + 1), (FFT_Input_Buf[Fx_Index[0] << 1] + 0));
    for (i = 1; i < 5; ++i)
    {
        /* 找出谐波位置 */
        Fx_Index[i] = Max_Float_WithinRange(
            Am_Data_Pointer,
            Fx_Index[0] * (i + 1) - (FFT_To_Am_IndexErrorRange >> 1),
            Fx_Index[0] * (i + 1) + (FFT_To_Am_IndexErrorRange >> 1)); // 在一定范围查找 较为准确

        /* 相位计算 */
        Phase_Pointer[i] = atan2f((FFT_Input_Buf[Fx_Index[i] << 1] + 1), (FFT_Input_Buf[Fx_Index[i] << 1] + 0));

        /* 幅值计算 */
        Fx_Vpp_Pointer[i] = Am_Data_Pointer[Fx_Index[i]] * 4 / ADC_SAMPLING_NUM;

        /* 归一化幅值计算 */
        NormAm_Pointer[i - 1] = Am_Data_Pointer[Fx_Index[i]] / Am_Data_Pointer[Fx_Index[0]];

        /* THDx部分计算 */
        Square_Sum += Am_Data_Pointer[Fx_Index[i]] * Am_Data_Pointer[Fx_Index[i]]; // 平方和
    }
    arm_sqrt_f32(Square_Sum, THD_Pointer); // 开根号
    *THD_Pointer = *THD_Pointer * 100 / Am_Data_Pointer[Fx_Index[0]];

    log_Fn_NAm_THD_data(Fx_Index, Phase_Pointer, NormAm_Pointer, *THD_Pointer);
}

/* 用归一化幅值+各分量相位 还原波形 */
void Restore_Waveform(u16 *RestoreWaveform_Pointer, float *NormAm_Pointer, float *Phase_Pointer)
{
    log_detail("Transforming Normalized Am To Waveform Data...\r\n");

    Signal_Synthesizer(RestoreWaveform_Pointer, OLED_X_MAX, 256, // 这个256是随便定的，目的是把小数转换为整数，使得波形细腻；OLED显示函数会进一步处理范围
                       NormAm_Pointer, (void *)0, 5);

    log_detail("Transforming Completed!\r\n");
}

/* 用幅值+各分量相位 还原波形 */
void Restore_Waveform_By_Vpp(u16 *RestoreWaveform, u16 *Fx_Vpp, float *Phase)
{
    log_detail("Transforming Normalized Am To Waveform Data...\r\n");

    Signal_Synthesizer_Vpp(RestoreWaveform, OLED_X_MAX, Fx_Vpp, (void *)0, 5);

    log_detail("Transforming Completed!\r\n");
}
