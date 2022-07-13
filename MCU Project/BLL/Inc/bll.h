#ifndef __BLL_H__
#define __BLL_H__
#include "config.h"

void Signal_F0_Measure(u32 *Captured_Value);
void Signal_Fs_Adjust(u32 Captured_Value);
void SignalSample_Start(u16 *Data);
void Bluetooth_SendDate_To_Phone(float *NormalizedAm, float THD, u16 *WaveformData);

#endif
