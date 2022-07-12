#ifndef __STIMULATION_H
#define __STIMULATION_H
#include "config.h"

#define Simulate_Sample_ADC_Noise 100
#define Simulate_WaveformDate_Period_Length SignalSampleFreq_Multiple

extern u8 Simulation_Times_Index;

void Simulate_Signal_WaveformData(u16 *SimulateWaveData);
void Simulate_Signal_Synthesizer(u16 *SimulateWaveData);

#endif
