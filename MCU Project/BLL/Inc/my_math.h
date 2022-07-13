#ifndef __MY_MATH
#define __MY_MATH
#include "config.h"

#define Signal_Synthesizer_Wave_Length_MAX 180

/* 找出最小值位置 */
u16 Min_Float(float Mag[], u16 len);

/* 找出最大值位置 */
u16 Max_Unsigned(u16 Mag[], u16 len);

/* 在一定范围内找出最大值位置 */
u16 Max_Float_WithinRange(float Data[], u16 Left, u16 Right);

/* 通过FFT 计算各个频率分量幅值 */
void CalculateAmplitude_By_FFT(float *Am_Data, u16 *SampleData);

/*  计算各个频率分量幅值 */
void NormalizedAm_And_CalculateTHD(float *NormAm, float *THD, float *Am_Data);

/* 将归一化幅值转化为波形数据 */
void Transform_NormalizedAm_To_WaveformData(float *NormAm, u16 *WaveformData);

/**
 * @brief  信号合成器
 * @param[out] Output        波形数据输出指针
 * @param[in]  Magnification 放大倍率
 * @param[in]  NormAm        归一化幅值
 * @param[in]  Precision     最高几次谐波分量
 */
void Signal_Synthesizer(u16 *Output, u16 Length, u16 Magnification, float *NormAm, u8 Precision);

#endif
