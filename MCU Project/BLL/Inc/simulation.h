#ifndef __STIMULATION_H
#define __STIMULATION_H
#include "config.h"

#define SIMULATION_TIMES 7          // 仿真次数
#define SIMULATE_SAMPLE_ADC_NOISE 1 // ADC采样过程噪声
#define SIMULATION_CCR_MAX 0xFFFF   // 定时器最大值 16位为 0xFFFF 32位为 0xFFFFFFFF
#define SIMULATION_CCR (SIMULATION_CCR_MAX & Simulation_CCR_Data[Simulation_Times_Index])

extern u8 Simulation_Times_Index;                 // 仿真次数索引
extern u32 Simulation_CCR_Data[SIMULATION_TIMES]; // 仿真捕获值

void Simulation_Set_Fs_ARR(u16 Fs_ARR);
void Simulate_Signal_Synthesizer(u16 *SimulateWaveData, u16 Length); // 信号合成
void Simulate_Signal_WaveformData(u16 *SimulateWaveData);            // 常见信号

#endif
