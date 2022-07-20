#include "log.h"
#include "oled_interface.h"
#include "arm_math.h"

#if Simulation
#define Signal_Captured_Period Simulation_CCR
#else
#include "bsp_it.h"
#define Signal_Captured_Period BSP_Signal_Capture_Value
#endif

void log_Fs_data(u32 F0_CCR, u32 Fs_CCR, u8 Flag)
{
    if (Flag)
    {
        log_debug("Fs CCR: %u\r\n", Fs_CCR);
        log_debug("Fs: %ukHz(Equivalent), %ukHz(Actual)\r\n", SignalSampleFreq_Multiple * TimerSourerFreq / 1000 / F0_CCR, TimerSourerFreq / 1000 / Fs_CCR);
    }
    else
    {
        log_debug("Fs CCR: %u\r\n", Fs_CCR);
        log_debug("Fs: %ukHz\r\n", TimerSourerFreq / 1000 / Fs_CCR);
    }
}

#define Fs (SignalSampleFreq_Multiple * TimerSourerFreq / Signal_Captured_Period)
#define FFT_Freq_Calculate(Index) (Index * Fs / ADC_SAMPLING_NUM)
void log_Fn_NAm_THD_data(u16 *Fx_Index, float *NormAm, float THD)
{
    // u8 i;
    log_debug("F0: %ukHz\r\n", FFT_Freq_Calculate(Fx_Index[0]) / 1000);
    // for (i = 1; i < 5; ++i)
    //      log_debug("F%u: %ukHz\r\n", (i + 1), FFT_Freq_Calculate(Fx_Index[i]) / 1000);
    log_debug("Normalized Am: 1.000, %0.3f, %0.3f, %0.3f, %0.3f\r\n", NormAm[0], NormAm[1], NormAm[2], NormAm[3]); // 归一化幅值
    log_debug("THDx: %.3f%%\r\n\r\n", THD);
}

void log_data_to_draw_stamp(u16 *Signal_ADC_Data, float *Amplitude_Data,
                            u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                            float THDx, u32 Signal_Captured_Value)
{
    u16 i;
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
        log_draw_stamp(ADC, i, "%u", Signal_ADC_Data[i]);
    log_draw_stamp(ADC, i, "%u", Signal_ADC_Data[0]);

    for (i = 0; i < (ADC_SAMPLING_NUM >> 1); ++i)
        log_draw_stamp(AM, i, "%.3f", Amplitude_Data[i]);

    for (i = 0; i < OLED_X_MAX; ++i)
        log_draw_stamp(RSWave, i, "%u", WaveformData_Restored[i]);

    // log_draw_ascii_blank(Phase, 10);
    // for (i = 0; i < 5; ++i)
    //     log_draw_ascii(Phase, "%0.3f", (Phase[i] * 180 / PI));

    // log_draw_ascii_blank(Norm AM, 10);
    // log_draw_ascii(Norm AM, "1.000");
    // for (i = 0; i < 4; ++i)
    //     log_draw_ascii(Norm AM, "%0.3f", NormalizedAm[i]);
}

void log_data_to_draw_ascii(u16 *Signal_ADC_Data, float *Amplitude_Data,
                            u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                            float THDx, u32 Signal_Captured_Value)
{
    u16 i;
    u16 log_times_temp;
    log_draw_ascii_blank(ADC, 50, "%u", Signal_ADC_Data[0]);
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
        log_draw_ascii(ADC, "%u", Signal_ADC_Data[i]);
    log_draw_ascii(ADC, "%u", Signal_ADC_Data[0]);

    log_draw_ascii_blank(AM, 50, "0");
    for (i = 0; i < (ADC_SAMPLING_NUM >> 1); ++i)
        log_draw_ascii(AM, "%.3f", Amplitude_Data[i]);

    log_draw_ascii_blank(RSWave, 25, "%u", WaveformData_Restored[0]);
    for (i = 0; i < OLED_X_MAX; ++i)
        log_draw_ascii(RSWave, "%u", WaveformData_Restored[i]);

    for (i = 0; i < 5; ++i)
    {
        log_draw_ascii(Phase, "%0.3f", (Phase[i] * 180 / PI));
        log_draw_ascii_blank(Phase, 9, "0");
    }

    log_draw_ascii(NormAM, "1.000");
    for (i = 0; i < 4; ++i)
    {
        log_draw_ascii_blank(NormAM, 9, "0");
        log_draw_ascii(NormAM, "%.3f", NormalizedAm[i]);
    }
    log_draw_ascii_blank(NormAM, 9, "0");
}

void log_data_to_draw(u16 *Signal_ADC_Data, float *Amplitude_Data,
                      u16 *WaveformData_Restored, float *NormalizedAm, float *Phase,
                      float THDx, u32 Signal_Captured_Value)
{
    u16 i;
    printf("\r\n\r\n***********************  ***  ****************************\r\n\r\n");

    log_indata("ADC Sampling Data:\r\n");
    for (i = 0; i < (ADC_SAMPLING_NUM); ++i)
        log_indata("%u\r\n", Signal_ADC_Data[i]);
    log_indata("%u\r\n", Signal_ADC_Data[0]);
    log_indata("\r\n*********************\r\n");

    log_indata("Am Data(a half):\r\n");
    for (i = 0; i < (ADC_SAMPLING_NUM >> 1); ++i)
        log_indata("%.3f\r\n", Amplitude_Data[i]);
    log_indata("\r\n*********************\r\n");

    log_indata("Waveform Data:\r\n");
    for (i = 0; i < OLED_X_MAX; ++i)
        log_indata("%u\r\n", WaveformData_Restored[i]);
    log_indata("\r\n*********************\r\n");

    log_indata("Phase Data:\r\n");
    for (i = 0; i < 5; ++i)
        log_indata("%0.3f\r\n", (Phase[i] * 180 / PI));
    log_indata("\r\n*********************\r\n");

    log_indata("Normalized Am Data:\r\n"); // 归一化幅值
    log_indata("1.000\r\n");
    for (i = 0; i < 4; ++i)
        log_indata("%0.3f\r\n", NormalizedAm[i]);
    log_indata("\r\n*********************\r\n");

    log_indata("THDx:\r\n"); // THDx
    log_indata("%0.3f\r\n", THDx);
    log_indata("\r\n*********************\r\n");

    log_indata("Period:(us)\r\n"); // 信号周期
    log_indata("%u\r\n", Signal_Captured_Value * 1000000 / TimerSourerFreq);
}

#ifdef USE_HAL_DRIVER
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int _io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
#endif
