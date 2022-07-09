#ifndef __CONFIG_H
#define __CONFIG_H

#define ADC_SAMPLING_NUM 1024
#define TimerSourerFreq 48000000
#define SignalSample_Freq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSample_Freq_MAX 1000000
#define SignalSample_Period_MIN (TimerSourerFreq / SignalSample_Freq_MAX)

#define DEBUG

#endif
