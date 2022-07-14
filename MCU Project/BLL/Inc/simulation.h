#ifndef __STIMULATION_H
#define __STIMULATION_H
#include "config.h"

#define Simulation_Times 7            // 仿真次数
#define Simulate_Sample_ADC_Noise 32  // ADC采样过程噪声
#define Simulation_CCR_MAX 0xFFFFFFFF // 定时器最大值 16位为 0xFFFF 32位为 0xFFFFFFFF
#define Simulation_CCR (Simulation_CCR_MAX & Simulation_CCR_Data[Simulation_Times_Index])

extern u8 Simulation_Times_Index;  // 仿真次数索引
extern u32 Simulation_CCR_Data[7]; // 仿真捕获值

void Simulation_Set_Fs_ARR(u16 Fs_ARR);
void Simulate_Signal_Synthesizer(u16 *SimulateWaveData, u16 Length); // 信号合成
void Simulate_Signal_WaveformData(u16 *SimulateWaveData);            // 常见信号

#endif
