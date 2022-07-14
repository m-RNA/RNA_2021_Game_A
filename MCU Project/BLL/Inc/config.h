#ifndef __CONFIG_H
#define __CONFIG_H
#include "oled_config.h"

// 是否调试
#define DEBUG (1u)
#define DEBUG_PRINT_INTERNAL_DATA (1u)
#define Simulation (1u)

#ifdef USE_HAL_DRIVER
#include "main.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"

#define BLUETOOTH_UART &huart2
#define SIGNAL_CAPTURE_TIMER &htim2
#define SIGNAL_SAMPLE_TIMER &htim3
#define SIGNAL_SAMPLE_TIMER_CHANNEL TIM_CHANNEL_1
#define SIGNAL_SAMPLE_TIMER_ACTIVE_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1
#define SIGNAL_SAMPLE_ADC &hadc1
#define OLED_Internal_IIC &hi2c1

#define delay_ms(MS) HAL_Delay(MS)

#elif defined __MSP432P401R__
#include "sysinit.h"
#include "timA.h"
#include "delay.h"
#include "adc.h"
#include "usart.h"
#include "usart3.h"
#include "led.h"

#define BLUETOOTH_UART EUSCI_A2_BASE
#define SIGNAL_CAPTURE_TIMER TIMER_A2_BASE                              //在这里改定时器
#define SIGNAL_CAPTURE_TIMER_REGISTER TIMER_A_CAPTURECOMPARE_REGISTER_2 //在这里改定时器通道
#define SIGNAL_CAPTURE_TIMER_PORT_PIN GPIO_PORT_P5, GPIO_PIN7           //在这里改复用引脚
#define SIGNAL_SAMPLE_TIMER TIMER_A0_BASE

#else
#error Doesn't contain top-level header file
#endif

#if defined __STM32F1xx_HAL_H
#define TimerSourerFreq 72000000u
#define SignalSampleFreq_MAX 1000000u

#elif defined STM32G431xx
#define TimerSourerFreq 170000000u
#define SignalSampleFreq_MAX 2000000u

#elif defined __MSP432P401R__
#define TimerSourerFreq 48000000u
#define SignalSampleFreq_MAX 1000000u

//#define HC_05_USART_PORT_PIN GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3

#endif

#define ADC_SAMPLING_NUM 1024u
#define SignalSampleFreq_Multiple 16u // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSamplePeriod_MIN (TimerSourerFreq / SignalSampleFreq_MAX)

#if (TimerSourerFreq >= 0xFFFF * 1000)
//#warning In this version, it is better for TimerSourerFreq to be Lower than 65535000.
#endif

#if Simulation
#include "simulation.h"
#endif

#endif
