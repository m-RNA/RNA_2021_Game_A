#ifndef __MY_MATH
#define __MY_MATH
#include "config.h"

/* 找出最小值位置 */
u16 Min_Float(float Mag[], u16 len);

/* 找出最大值位置 */
u16 Max_Unsigned(u16 Mag[], u16 len);

/* 在一定范围内找出最大值位置 */
u16 Max_Float_WithinRange(float Data[], u16 Left, u16 Right);

/* 通过FFT 计算各个频率分量幅值 */
void CalculateAmplitude_By_FFT(u16 *SampleData, float *Am_Data);

/**
 * @brief  信号合成器
 * @param[out] Output        波形数据输出指针
 * @param[in]  Magnification 放大倍率
 * @param[in]  NormAm        归一化幅值
 * @param[in]  Precision     最高几次谐波分量
 */
void Signal_Synthesizer(u16 *Output, u16 Length, u16 Magnification, float *NormAm, u8 Precision);

#endif
