#ifndef __LOG_H__
#define __LOG_H__
#include "config.h"
#include "stdio.h"

#if ENABLE_LOG_DETAIL
#define log_detail(fmt, ...)        \
    do                              \
    {                               \
        printf("[Log Detail] ");    \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define log_detail(fmt, ...) ((void)0)
#endif

#if ENABLE_LOG_DEBUG
#define log_debug(fmt, ...)         \
    do                              \
    {                               \
        printf("[Log Debug] ");     \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
#define log_debug(fmt, ...) ((void)0)
#endif

#if ENABLE_LOG_DRAW
#define log_indata(fmt, ...)        \
    do                              \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    } while (0)

#define log_draw_ascii(title, fmt, ...)                  \
    do                                                   \
    {                                                    \
        printf("{" #title ":" fmt "}\n", ##__VA_ARGS__); \
    } while (0)

#define log_draw_ascii_blank(title, length, fmt, ...)    \
    log_times_temp = 0;                                  \
    do                                                   \
    {                                                    \
        ++log_times_temp;                                \
        printf("{" #title ":" fmt "}\n", ##__VA_ARGS__); \
    } while (log_times_temp < length)

#define log_draw_stamp(title, timestamp, fmt, ...) \
    do                                             \
    {                                              \
        printf("<"                                 \
               "%u"                                \
               ">"                                 \
               "{" #title "}" fmt "\n",            \
               timestamp, ##__VA_ARGS__);          \
    } while (0)

#define log_draw_blank_stamp(title, timestamp, length) \
    i = timestamp;                                     \
    do                                                 \
    {                                                  \
        ++i;                                           \
        printf("<"                                     \
               "%u"                                    \
               ">"                                     \
               "{" #title "}0\n",                      \
               timestamp);                             \
    } while (i < length + timestamp)
#else
#define log_indata(fmt, ...) ((void)0)
#define log_draw_ascii(title, fmt, ...) ((void)0)
#define log_draw_ascii_blank(title, length, fmt, ...) ((void)0)
#define log_draw_stamp(title, timestamp, fmt, ...) ((void)0)
#define log_draw_blank_stamp(title, timestamp, length) ((void)0)
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
void log_Fn_NAm_THD_data(u16 *Fx_Index,float * Phase, float *NormAm, float THD);
void log_data_to_draw(u16 *Signal_ADC_Data, float *Amplitude_Data,
                      u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                      float THDx, u32 Signal_Captured_Value);
void log_data_to_draw_ascii(u16 *Signal_ADC_Data, float *Amplitude_Data,
                            u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                            float THDx, u32 Signal_Captured_Value);
void log_data_to_draw_stamp(u16 *Signal_ADC_Data, float *Amplitude_Data,
                            u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                            float THDx, u32 Signal_Captured_Value);

#endif
