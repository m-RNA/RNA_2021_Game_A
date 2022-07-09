#include "bll.h"
#include "bsp.h"
#include "log.h"
#include "config.h"
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

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Fs_CCR = 0;
    Fs_CCR = Captured_Value / SignalSampleFreq_Multiple;

    if (Captured_Value <= SignalSamplePeriod_MIN)
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
    u16 i;
    float fft_inputbuf[ADC_SAMPLING_NUM * 2];
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[0 + (i << 1)] = SampleData[i]; // 实部为ADC
        fft_inputbuf[1 + (i << 1)] = 0;             // 虚部为0
    }

    // arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    // arm_cmplx_mag_f32(fft_inputbuf, Output, ADC_SAMPLING_NUM);  //把运算结果复数求模得幅值
}

/* 在一定范围内找出最大值位置 */
u16 FloatMaxIndex_WithinRange(float Data[], u16 Left, u16 Right) // 最优算法应为分治法
{
    u16 i, MaxIndex;
    MaxIndex = Left;
    for (i = Left; i <= Right; ++i)
    {
        if (Data[MaxIndex] < Data[i])
        {
            MaxIndex = i;
        }
    }
    return MaxIndex;
}

#define FFT_To_Am_IndexErrorRange 4
void NormalizedAm_And_CalculateTHD(float *Am_Data, float *NormAm, float *THD)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float sum = 0.0f;

    /* 找出基波位置 */
    Fx_Index[0] = FloatMaxIndex_WithinRange(Am_Data, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));

    for (i = 0; i < 4; ++i)
    {
        /* 找出谐波位置 */
        Fx_Index[i + 1] = FloatMaxIndex_WithinRange(Am_Data, Fx_Index[0] * (i + 2) - (FFT_To_Am_IndexErrorRange >> 1), Fx_Index[0] * (i + 2) + (FFT_To_Am_IndexErrorRange >> 1)); // 优化过的算法 更加准确

        /* 计算归一化幅值 */
        NormAm[i] = floor(Am_Data[Fx_Index[i + 1]] / Am_Data[Fx_Index[0]] * 100.0f) / 100.0f; // 向下取整floor() 误差更小
    }

    /* THDx计算 */
    for (i = 0; i < 4; ++i)
    {
        sum += Am_Data[Fx_Index[i + 1]] * Am_Data[Fx_Index[i + 1]];
    }
    *THD = ceil(sqrt(sum) / Am_Data[Fx_Index[0]] * 10000) / 100.0f; // 向上取整ceil()
}

// 找出最大值位置
u16 Compare_Max(float Mag[], u16 len)
{
    u16 i, Fn_Num;
    Fn_Num = 0;
    Mag[Fn_Num] = Mag[0];
    for (i = 1; i < len; i++)
    {
        if (Mag[Fn_Num] < Mag[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}

// 找出最小值位置
u16 Compare_Min(float Mag[], u16 len)
{
    u16 i, Fn_Num;
    Fn_Num = 0;
    Mag[Fn_Num] = Mag[0];
    for (i = 1; i < len; i++)
    {
        if (Mag[Fn_Num] > Mag[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}

#define X_MAX 128
#define Y_MAX 64
#define Y_MULTI 54
#define Y_UP_MOVE 64
void Transform_NormAm_To_WaveformData(float *NormAm, u16 *WaveformData)
{
    u16 i;
    u16 minNum;
    u16 maxNum;
    u16 vm;
    vu16 temp_last, temp;
    float OriginalWaveDate[X_MAX];
    for (int i = 0; i < X_MAX; ++i)
    {
        OriginalWaveDate[i] = Y_MULTI * sin(PI * i / 64.0f);
        // OriginalWaveDate[i] = Y_MULTI * arm_sin_f32(PI * i / 64.0f);
        for (int j = 0; j < 4; ++j)
        {
            OriginalWaveDate[i] += Y_MULTI * sin(PI * i * (j + 1) / 64.0f) * NormAm[j];
            // OriginalWaveDate[i] += Y_MULTI * arm_sin_f32(PI * i * (j + 1) / 64.0f) * NormAm[j];
        }
        OriginalWaveDate[i] += Y_UP_MOVE;
        log_debug("%f\n", OriginalWaveDate[i]);
    }

    minNum = Compare_Min(OriginalWaveDate, X_MAX);
    maxNum = Compare_Max(OriginalWaveDate, X_MAX);
    vm = OriginalWaveDate[maxNum] - OriginalWaveDate[minNum];
    // SelectUpOLED();
    // ClearScreen();
    // WaveBox();
    WaveformData[0] = OriginalWaveDate[0] - OriginalWaveDate[minNum];
    // DrawPixel(i, temp_last);
    WaveformData[1] = OriginalWaveDate[1] - OriginalWaveDate[minNum];
    temp_last = (u16)(64 - WaveformData[1] * 54 / (float)vm - 5);
    for (i = 2; i < X_MAX; ++i)
    {
        WaveformData[i] = OriginalWaveDate[i] - OriginalWaveDate[minNum];
        temp = (u16)(64 - WaveformData[i] * 54 / (float)vm - 5);
        // printf("%d,", WaveformData[i]);
        // DrawLine(i, temp_last, i + 1, temp);
        // printf("%d\r\n", temp);
    }
    // UpdateScreen();
}
