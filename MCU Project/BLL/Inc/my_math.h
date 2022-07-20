#ifndef __MY_MATH
#define __MY_MATH
#include "config.h"

#ifdef __MSP432P401R__
#if (ADC_SAMPLING_NUM < 4096u)
#define Signal_Synthesizer_Wave_Length_MAX ADC_SAMPLING_NUM
#else
#define Signal_Synthesizer_Wave_Length_MAX 1024u
#endif
#elif defined __STM32F1xx_HAL_H
#if (ADC_SAMPLING_NUM < 1024u)
#define Signal_Synthesizer_Wave_Length_MAX ADC_SAMPLING_NUM
#else
#define Signal_Synthesizer_Wave_Length_MAX 170u
#endif
#else
#warning...
#endif

/* 找出最小值位置 */
u16 Min_Float(float Mag[], u16 len);

/* 找出最大值位置 */
u16 Max_Unsigned(u16 Mag[], u16 len);

/* 在一定范围内找出最大值位置 */
u16 Max_Float_WithinRange(float Data[], u16 Left, u16 Right);

/* 通过FFT 计算各个频率分量幅值 */
void CalculateAmplitude_By_FFT(float *Am_Pointer, u16 *SampleData_Pointer);

/*  计算各个谐波分量幅值相位 */
void NormalizedAm_And_CalculateTHD(float *Phase_Pointer, float *NormAm_Pointer, u16 *Fx_Vpp_Pointer, float *THD_Pointer, float *Am_Data_Pointer);

/* 用归一化幅值+各分量相位 还原波形 */
void Restore_Waveform(u16 *RestoreWaveform_Pointer, float *NormAm_Pointer, float *Phase_Pointer);

/* 用幅值+各分量相位 还原波形 */
void Restore_Waveform_By_Vpp(u16 *RestoreWaveform, u16 *Fx_Vpp, float *Phase);

/**
 * @brief  信号合成器
 * @param[out] Output    波形数据输出指针
 * @param[in]  Length    波形数据输出长度
 * @param[in]  F0_Vpp    基波幅值(mv)
 * @param[in]  NormAm    归一化幅值
 * @param[in]  Phase     相位
 * @param[in]  Precision 最高几次谐波分量
 */
void Signal_Synthesizer(u16 *Output, u16 Length, u16 F0_Vpp, float *NormAm, float *Phase, u8 Precision);

/**
 * @brief  信号合成器
 * @param[out] Output    波形数据输出指针
 * @param[in]  Length    波形数据输出长度
 * @param[in]  Fx_Vpp    基波-谐波幅值(mv)
 * @param[in]  Phase     相位(弧度)
 * @param[in]  Precision 最高几次谐波分量
 */
void Signal_Synthesizer_Vpp(u16 *Output, u16 Length, u16 *Fx_Vpp, float *Phase, u8 Precision);

#endif
