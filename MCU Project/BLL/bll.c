#include "bll.h"
#include "bsp.h"
#include "log.h"
#include "config.h"
#include "stdio.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "led.h"
#include "adc.h"

void Signal_Sample_Init(void)
{
    BSP_Sample_ADC_with_DMA_Init();
    BSP_Sample_Timer_Init();
}

void Signal_F0_Measure(u32 *Captured_Value)
{
    LED_Y_On();
    log_debug("Signal F0 Measuring...\r\n");
    *Captured_Value = BSP_Get_Signal_CCR();
    log_debug("F0 Captured Value:%u\r\n", *Captured_Value);
    log_debug("F0(Captured):%uHz\r\n", TimerSourerFreq / (*Captured_Value));
    LED_W_Off();
}

void Signal_Fs_Adjust(u32 Captured_Value)
{
    u32 Fs_CCR = 0;
    LED_R_On();
    log_debug("Signal Fs Adjusting...\r\n");

    Fs_CCR = Captured_Value / SignalSampleFreq_Multiple;

    if (Captured_Value <= SignalSamplePeriod_MIN)
    {
        Fs_CCR += Captured_Value;
        log_debug("Using Equivalent Eampling!\r\n");
    }
    BSP_Set_Fs_CCR(Fs_CCR);
    log_debug("Fs CCR:%u\r\n", Fs_CCR);
    log_debug("Fs:%uHz\r\n", TimerSourerFreq / Fs_CCR);
    LED_W_Off();
}

void SignalSample_Start(u16 *Data)
{
    LED_RED_Off();
    LED_W_Off();
    log_debug("Signal Sampling...\r\n");
    BSP_ADC_DMA_Start(Data, ADC_SAMPLING_NUM);
    
#ifdef DEBUG_PRINT_INTERNAL_DATA
    log_debug("ADC Sampling Data:\r\n");
    for(u16 i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        printf("%u\r\n",Data[i]);
    }
#endif
    log_debug("Signal Sample Completed!\r\n");
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
    LED_W_On();
    log_debug("Calculating Amplitudes...\r\n");

    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        fft_inputbuf[0 + (i << 1)] = SampleData[i]; // 实部为ADC
        fft_inputbuf[1 + (i << 1)] = 0;             // 虚部为0
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1); // FFT计算
    arm_cmplx_mag_f32(fft_inputbuf, Output, ADC_SAMPLING_NUM);  //把运算结果复数求模得幅值
    
#ifdef DEBUG_PRINT_INTERNAL_DATA    
    printf("Am Data:\r\n");
    for (i = 0; i < ADC_SAMPLING_NUM; ++i)
    {
        // printf("[%d]:%.3f\r\n", i, fft_outputbuf[i]);
    }
#endif
    LED_W_Off();
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
#define FFT_Freq_Calculate(Index) (Index * TimerSourerFreq / true_T / ADC_SAMPLING_NUM)
void NormalizedAm_And_CalculateTHD(float *Am_Data, float *NormAm, float *THD)
{
    u16 i;
    u16 Fx_Index[5] = {0};
    float sum = 0.0f;
    LED_G_On();

    /* 找出基波位置 */
    Fx_Index[0] = FloatMaxIndex_WithinRange(Am_Data, 1 + (FFT_To_Am_IndexErrorRange >> 1), (ADC_SAMPLING_NUM >> 1));
    log_debug("F0 Freq: %uHz\r\n",FFT_Freq_Calculate(Fx_Index[0]));

    for (i = 0; i < 4; ++i)
    {
        /* 找出谐波位置 */
        Fx_Index[i + 1] = FloatMaxIndex_WithinRange(Am_Data, Fx_Index[0] * (i + 2) - (FFT_To_Am_IndexErrorRange >> 1), Fx_Index[0] * (i + 2) + (FFT_To_Am_IndexErrorRange >> 1)); // 优化过的算法 更加准确
        log_debug("F%u Freq: %uHz\r\n",(i + 2), FFT_Freq_Calculate(Fx_Index[i]));

        /* 计算归一化幅值 */
        NormAm[i] = floor(Am_Data[Fx_Index[i + 1]] / Am_Data[Fx_Index[0]] * 100.0f) / 100.0f; // 向下取整floor() 误差更小
    }
    
    log_debug("Normalized Am Data: 1, "); // 归一化幅值

    /* THDx计算 */
    for (i = 0; i < 4; ++i)
    {
#ifdef DEBUG
        printf("%0.3f, ",NormAm[i]);  // 借用 打印归一化幅值
#endif
        sum += Am_Data[Fx_Index[i + 1]] * Am_Data[Fx_Index[i + 1]];
    }
#ifdef DEBUG
        printf("\r\n");
#endif
    
    *THD = ceil(sqrt(sum) / Am_Data[Fx_Index[0]] * 10000) / 100.0f; // 向上取整ceil()
    log_debug("THDx: %.2f%%\r\n",*THD);

    LED_W_Off();
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

#define SIN(Cita) arm_sin_f32(Cita)

void Transform_NormalizedAm_To_WaveformData(float *NormAm, u16 *WaveformData)
{
    u16 i;
    u16 MinIndex;
    float OriginalWaveDate[OLED_X_MAX];
    LED_P_On();    
    log_debug("Transforming Normalized Am To Waveform Data...\r\n");

    
    for (int i = 0; i < OLED_X_MAX; ++i)
    {
        OriginalWaveDate[i] = SIN(PI * i / ((float)(OLED_X_MAX >> 1)));
        for (int j = 0; j < 4; ++j)
        {
            OriginalWaveDate[i] += SIN(PI * i * (j + 1) / ((float)(OLED_X_MAX >> 1))) * NormAm[j];
        }
        log_debug("%f\n", OriginalWaveDate[i]);
    }
    
    // 找出最小的小数的位置
    MinIndex = Compare_Min(OriginalWaveDate, OLED_X_MAX);

    for (i = 0; i < OLED_X_MAX; ++i)
    {
        // 将小数全转为为正数，再乘以100变为整数
        WaveformData[i] = 100 * (OriginalWaveDate[i] - OriginalWaveDate[MinIndex]);
        // 这个100是随便定的，不要太大就好了，目的是把小数转换为整数；OLED显示函数会进一步处理范围
    }
    log_debug("Transforming Completed!\r\n");
    LED_W_Off();
}

void Bluetooth_SendByte(u8 Data)
{
#if defined __STM32__
    HAL_UART_Transmit(BLUETOOTH_UART, &Data, 1, 50);
#elif defined __MSP432P401R__
    MAP_UART_transmitData(BLUETOOTH_UART, Data);
#endif
}

void Bluetooth_SendDate_To_Phone(float *NormalizedAm, float THDx, u16 *WaveformData)
{
	uint8_t i;
    LED_B_On();
    log_debug("Bluetooth Sending Date To Phone.\r\n");

	/* 发送THD */
	Bluetooth_SendByte(((uint16_t)(THDx * 100)) >> 8);
	Bluetooth_SendByte(((uint16_t)(THDx * 100)) & 0xFF);

	/* 发送拟合值 */
	for (i = 0; i < 128; ++i)
	{
		Bluetooth_SendByte(WaveformData[i] >> 8);
		Bluetooth_SendByte(WaveformData[i] & 0xFF);
	}

	/* 发送归一化幅值 */
	for (i = 0; i < 4; ++i)
	{
		Bluetooth_SendByte(((uint16_t)(NormalizedAm[i + 1] * 100)) >> 8);
		Bluetooth_SendByte(((uint16_t)(NormalizedAm[i + 1] * 100)) & 0xFF);
	}
    log_debug("Bluetooth Sending Completed!\r\n");
    LED_W_Off();
}


