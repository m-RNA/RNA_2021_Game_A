#ifndef __CONFIG_H
#define __CONFIG_H
#include "oled_config.h"

// #define __STM32__

#if defined STM32F103xE || STM32F103xB
#define TimerSourerFreq 72000000
#define SignalSampleFreq_MAX 1000000
#define delay_ms(MS) HAL_Delay(MS)

#include "usart.h"
#define BLUETOOTH_UART &huart2

#elif defined STM32G431xx
#define TimerSourerFreq 170000000
#define SignalSampleFreq_MAX 2000000
#define delay_ms(MS) HAL_Delay(MS)

#elif defined __MSP432P401R__
#define TimerSourerFreq 48000000
#define SignalSampleFreq_MAX 1000000

#define BLUETOOTH_UART EUSCI_A2_BASE
//#define HC_05_USART_PORT_PIN GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3

#endif

#define ADC_SAMPLING_NUM 1024
#define SignalSampleFreq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSamplePeriod_MIN (TimerSourerFreq / SignalSampleFreq_MAX)

// 是否调试 是则定义 DEBUG
#define DEBUG (1u)
#define DEBUG_PRINT_INTERNAL_DATA (1u)

#if (ADC_SAMPLING_NUM != 1024)
#warning ADC_SAMPLING_NUM should be 1024
#endif

//#ifndef PI
//#define PI 3.1415926f
//#endif

#define OLED_X_MAX SCREEN_COLUMN
#define OLED_Y_MAX SCREEN_ROW

#endif
