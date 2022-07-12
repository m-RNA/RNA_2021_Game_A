#ifndef __CONFIG_H
#define __CONFIG_H
#include "oled_config.h"

// 是否调试 是则定义 DEBUG
#define DEBUG (1u)
#define DEBUG_PRINT_INTERNAL_DATA (1u)
#define Simulation

#ifdef USE_HAL_DRIVER
#include "main.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "gpio.h"

#define BLUETOOTH_UART &huart2
#define SIGNAL_CAPTURE_TIMER &htim2
#define SIGNAL_SAMPLE_TIMER &htim3
#define SIGNAL_SAMPLE_TIMER_CHANNEL TIM_CHANNEL_1
#define SIGNAL_SAMPLE_TIMER_ACTIVE_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1
#define SIGNAL_SAMPLE_ADC &hadc1

#define delay_ms(MS) HAL_Delay(MS)

#elif defined __MSP432P401R__
#include "sysinit.h"
#define BLUETOOTH_UART EUSCI_A2_BASE
#else
#error Doesnt contain top-level header file
#endif

#if defined __STM32F1xx_HAL_H
#define TimerSourerFreq 72000000
#define SignalSampleFreq_MAX 1000000
// #define true_T 1000

#elif defined STM32G431xx
#define TimerSourerFreq 170000000
#define SignalSampleFreq_MAX 2000000

#elif defined __MSP432P401R__
#define TimerSourerFreq 48000000
#define SignalSampleFreq_MAX 1000000

//#define HC_05_USART_PORT_PIN GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3

#endif

#define ADC_SAMPLING_NUM 1024
#define SignalSampleFreq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSamplePeriod_MIN (TimerSourerFreq / SignalSampleFreq_MAX)

#if (ADC_SAMPLING_NUM != 1024)
#warning ADC_SAMPLING_NUM should be 1024
#endif

#ifdef Simulation
#include "simulation.h"
#endif

#endif
