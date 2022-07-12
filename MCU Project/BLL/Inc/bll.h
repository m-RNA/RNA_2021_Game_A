#ifndef __BLL_H__
#define __BLL_H__
#include "config.h"

void System_Init(void);

void Signal_Sample_Init(void);
void Signal_F0_Measure(u32 *Captured_Value);
void Signal_Fs_Adjust(u32 Captured_Value);
void SignalSample_Start(u16 *Data);
void SignalSample_FFT_to_Am(u16 *Input, float *Output);
void NormalizedAm_And_CalculateTHD(float *Am_Data, float *NormAm, float *THD);
void Transform_NormalizedAm_To_WaveformData(float *NormAm, u16 *WaveformData);
void Bluetooth_SendDate_To_Phone(float *NormalizedAm, float THD, u16 *WaveformData);
void OLEDInterface_Update_Norm_Am_And_THD(float *NormalizedAm, float THD);

#endif
