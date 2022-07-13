#ifndef __STIMULATION_H
#define __STIMULATION_H
#include "config.h"

#define Simulation_Times 7           // 仿真次数
#define Simulate_Sample_ADC_Noise 64 // ADC采样噪声
#define Simulation_Capture_Timer_CCR_MAX 0xFFFF // 定时器最大值 16位为0xFFFF 32为0xFFFFFFFF
#define Simulation_CCR (Simulation_Capture_Timer_CCR_MAX & Simulation_CCR_Data[Simulation_Times_Index])

extern u8 Simulation_Times_Index;   // 仿真次数索引
extern u32 Simulation_CCR_Data[7]; // 仿真捕获值

void Simulation_Set_Fs_ARR(u16 Fs_ARR);
void Simulate_Signal_Synthesizer(u16 *SimulateWaveData);  // 信号合成
void Simulate_Signal_WaveformData(u16 *SimulateWaveData); // 常见信号

#endif
