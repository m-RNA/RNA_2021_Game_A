#ifndef __CONFIG_H
#define __CONFIG_H

#if defined STM32F103xE || STM32F103xB
#define TimerSourerFreq 72000000
#define SignalSampleFreq_MAX 1000000

#elif defined STM32G431xx
#define TimerSourerFreq 170000000
#define SignalSampleFreq_MAX 2000000

#elif defined __MSP432P401R__
#define TimerSourerFreq 48000000
#define SignalSampleFreq_MAX 1000000

#endif

#define ADC_SAMPLING_NUM 1024
#define SignalSampleFreq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSamplePeriod_MIN (TimerSourerFreq / SignalSampleFreq_MAX)

#define DEBUG

#if (ADC_SAMPLING_NUM != 1024)
#warning ADC_SAMPLING_NUM should be 1024
#endif

#endif
