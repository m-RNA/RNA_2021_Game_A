#ifndef __LOG_H__
#define __LOG_H__
#include "config.h"
#include "stdio.h"

#if DEBUG
#define log_debug(fmt, ...)         \
    do                              \
    {                               \
        printf("[Log Debug] ");     \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define log_debug(fmt, ...) ((void)0)
#endif

#if DEBUG_PRINT_INTERNAL_DATA
#define log_indata(fmt, ...)        \
    do                              \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)

#else
#define log_indata(fmt, ...) ((void)0)
#endif

#define log_assert(fmt, ...)                                                                           \
    do                                                                                                 \
    {                                                                                                  \
        printf("[Log Assert] File: %s, Function: %s, Line: %d\r\n", __FILE__, __FUNCTION__, __LINE__); \
        printf(fmt, ##__VA_ARGS__);                                                                    \
        while (1)                                                                                      \
            ;                                                                                          \
    } while (0)

void log_Fs_data(u32 F0_CCR, u32 Fs_CCR, u8 Flag);
void log_Fn_NAm_THD_data(u16 *Fx_Index, float *NormAm, float THD);
void log_Internal_data(u16 *Signal_ADC_Data, float *Amplitude_Data,
                       u16 *WaveformData_Restored, float *NormalizedAm,
                       float THDx, u32 Signal_Captured_Value);

#endif
