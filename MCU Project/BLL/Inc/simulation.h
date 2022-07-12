#ifndef __STIMULATION_H
#define __STIMULATION_H
#include "config.h"

#define Simulation_Times 7            // 仿真次数
#define Simulate_Sample_ADC_Noise 100 // ADC采样噪声

extern u8 Simulation_Times_Index; // 仿真次数索引
extern u32 Simulation_CCR[10];    // 仿真捕获值

void Simulate_Signal_Synthesizer(u16 *SimulateWaveData);  // 信号合成
void Simulate_Signal_WaveformData(u16 *SimulateWaveData); // 常见信号

#endif
