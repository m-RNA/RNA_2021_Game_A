#include "bll.h"
#include "bsp.h"
#include "log.h"
#include "stdio.h"
#include "math.h"

void Signal_Sample_Init(void)
{
    BSP_Sample_ADC_with_DMA_Init();
    BSP_Sample_Timer_Init();
}

void Signal_F0_Measure(u32 *Captured_Value)
{
    *Captured_Value = BSP_Get_Signal_CCR();
    log_debug("F0_CCR:%u\r\n", *Captured_Value);
}

#define TimerSourerFreq 48000000
#define SignalSample_Freq_Multiple 16 // 采样频率设定为信号基波频率的几倍（Fs = ？F0）
#define SignalSample_Freq_MAX 1000000
#define SignalSample_Period_MIN (TimerSourerFreq / SignalSample_Freq_MAX)

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Fs_CCR = 0;
    Fs_CCR = Captured_Value / SignalSample_Freq_Multiple;

    if (Captured_Value <= SignalSample_Period_MIN)
    {
        Fs_CCR += Captured_Value;
        printf("Oversampling!\r\n");
    }
    BSP_Set_Fs_CCR(Fs_CCR);
    log_debug("Fs_CCR:%u\r\n", Fs_CCR);
}

void SignalSample_Start(u16 *Data)
{
    BSP_ADC_DMA_Start(Data, ADC_SAMPLING_NUM);
}

void BLL_Init(void)
{
    log_debug("config BLL_Init...\r\n");
}

void System_Init(void)
{
    // HAL_Init();
    BSP_Init();
    BLL_Init();

    Signal_Sample_Init();
}

void SignalSample_FFT_to_Am(u16 *SampleData, float *Output)
{
    float fft_inputbuf[ADC_SAMPLING_NUM * 2];
    u16 i;
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[i << 1] = SampleData[i]; // 实部为ADC
        fft_inputbuf[i << 1 + 1] = 0;         // 虚部为0
    }
    // arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    // arm_cmplx_mag_f32(fft_inputbuf, Output, ADC_SAMPLING_NUM);  //把运算结果复数求模得幅值
}

/* 在一定范围内找出最大值位置 */
// 最优算法应为分治法
u16 FloatMax_Index_WithinRange(float Data[], u16 Left, u16 Right)
{
    u16 i, Fn_Num;
    Fn_Num = Left;
    for (i = Left; i <= Right; ++i)
    {
        if (Data[Fn_Num] < Data[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}
#define FFT_To_Am_IndexErrorRange 4

void Normalized_Am_Calculate_THD(float *Am_Data, float *Norm_Am, float *THD)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float sum = 0.0f;

    /* 找出基波位置 */
    Fx_Index[0] = FloatMax_Index_WithinRange(Am_Data, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));

    for (i = 0; i < 4; ++i)
    {
        /**   找出谐波位置   **/
        Fx_Index[i + 1] = FloatMax_Index_WithinRange(Am_Data, Fx_Index[0] * (i + 2) - (FFT_To_Am_IndexErrorRange >> 1), Fx_Index[0] * (i + 2) + (FFT_To_Am_IndexErrorRange >> 1)); // 优化过的算法 更加准确

        /**  计算归一化幅值  **/
        Norm_Am[i] = floor(Am_Data[Fx_Index[i + 1]] / Am_Data[Fx_Index[0]] * 100.0f) / 100.0f; // 向下取整 误差更小
    }

    /* THDx计算 */
    for (i = 0; i < 4; ++i)
    {
        sum += Am_Data[Fx_Index[i + 1]] * Am_Data[Fx_Index[i + 1]];
    }
    *THD = ceil(sqrt(sum) / Am_Data[Fx_Index[0]] * 10000) / 100.0f;
}
