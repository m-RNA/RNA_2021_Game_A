#ifndef __BLL_H__
#define __BLL_H__
#include "main.h"
#include "config.h"
void System_Init(void);

void Signal_Sample_Init(void);
void Signal_F0_Measure(u32 *Captured_Value);
void Signal_Fs_Adjust(u32 Captured_Value);
void SignalSample_Start(u16 *Data);
void SignalSample_FFT_to_Am(u16 *Input, float *Output);
void NormalizedAm_And_CalculateTHD(float *Am_Data, float *Norm_Am, float *THD);

#endif
