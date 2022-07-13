#include "simulation.h"
#include "arm_math.h"
#include "my_math.h"
#include "oled_interface.h"
#include "log.h"
#include "stdlib.h"

#define Simulate_WaveformDate_Period_Length SignalSampleFreq_Multiple
#define Add_Noise (rand() % Simulate_Sample_ADC_Noise)
u8 Simulation_Times_Index = 0;

#define Synthesize_Precision 13 // 精度 - 到几次谐波
float Simulation_NormAm[Simulation_Times][Synthesize_Precision - 1] = {
    {0.0f, 0.0f, 0.0f, 0.0f}, // 正弦波
    
    {0.00f, 0.20f, 0.00f, 0.15f}, // 电赛测试信号1
    {0.00f, 0.08f, 0.15f, 0.00f}, // 电赛测试信号2
    {0.00f, 0.00f, 0.00f, 0.10f}, // 电赛测试信号3


    {0.00f, -0.1111111111f, 0.00f, 0.04f, 0.0f, -0.0204081633f, 0.0f, 0.0123456790f,0.0f, -0.0082644628f, 0.0f, 0.0059171598f}, // 三角波
    {0.00f, 0.3333333333f, 0.0f, 0.2f, 0.0f, 0.1428571429f, 0.0f, 0.1111111111f, 0.0f,0.0909090909f, 0.0f,0.0769230769f},         // 方波
    {0.5f, 0.3333333333f, 0.25f, 0.2f, 0.1666666667f, 0.1428571429f, 0.125f, 0.1111111111f,0.1f,0.0909090909f,0.0833333333f,0.0769230769f}, // 锯齿波
};

u16 Simulation_F0_Vpp_Data[Simulation_Times] = {
    200, // 自定义
    
    400, // 电赛测试信号1
    200, // 电赛测试信号2
    30,  // 电赛测试信号3
    
    100, // 自定义
    100, // 自定义
    100, // 自定义
//    83,
//    65,
//    48
};

u32 Simulation_CCR_Data[Simulation_Times] = {
    0xFFFF,   // 自定义
    
    TimerSourerFreq / 1000,   // 电赛测试信号1
    TimerSourerFreq / 50000,  // 电赛测试信号2
    TimerSourerFreq / 100000, // 电赛测试信号3

    SignalSamplePeriod_MIN, // 最大采样率

    TimerSourerFreq / 300000, // 自定义
    TimerSourerFreq / 600000,
};

u16 Simulate_Fs_ARR = 0;
void Simulation_Set_Fs_ARR(u16 Fs_ARR)
{
    Simulate_Fs_ARR = Fs_ARR;
}

void Simulate_Signal_Synthesizer(u16 *SimulateWaveData)
{
    u16 Data_Index;
    u16 Freq_Multiple;
    extern u8 OverSamplingFlag;
    if(!OverSamplingFlag)
    {
        Freq_Multiple = Simulation_CCR_Data[Simulation_Times_Index] / Simulate_Fs_ARR;
    }
    else
    {
        Freq_Multiple = (SignalSampleFreq_Multiple + 1) * Simulation_CCR_Data[Simulation_Times_Index] / Simulate_Fs_ARR;
    }
    if(Freq_Multiple >= Signal_Synthesizer_Wave_Length_MAX)
        log_assert("Simulated ERROR: Freq_Multiple is too Big! Please check Simulation_CCR_Data to find that if Setting Frequency is too low.");
    
    Signal_Synthesizer(SimulateWaveData, Freq_Multiple, (Simulation_F0_Vpp_Data[Simulation_Times_Index] * 4096/ 3300) >> 1, 
                       Simulation_NormAm[Simulation_Times_Index], Synthesize_Precision);

    // 复制数据
    for (u16 i = 1; i <= ADC_SAMPLING_NUM / Freq_Multiple; ++i)
    {
        for (u16 j = 0; j < Freq_Multiple; ++j)
        {
            Data_Index = j + i * Freq_Multiple;
            SimulateWaveData[Data_Index] = SimulateWaveData[j] + Add_Noise;
            if (Data_Index >= ADC_SAMPLING_NUM)
                break;
        }
    }
    for (u16 j = 0; j < Freq_Multiple; ++j)
    {
        SimulateWaveData[j] += Add_Noise;
    }
}

/*******************************************************************/
static u16 Simulation_ADC_Data[Simulate_WaveformDate_Period_Length] = {0};
static void SquareWaveOut(void)
{
    u16 i;
    for (i = 0; i < Simulate_WaveformDate_Period_Length; i++)
    {
        if (i < Simulate_WaveformDate_Period_Length >> 1)
            Simulation_ADC_Data[i] = 4095;
        else
            Simulation_ADC_Data[i] = 0;
    }
}

// 三角波
static void TriangularWaveOut(void)
{
    u16 i, j;
    for (i = 0, j = 0; i < Simulate_WaveformDate_Period_Length; i++)
    {
        Simulation_ADC_Data[i] = j * 2 * 4095 / Simulate_WaveformDate_Period_Length;

        if (i < Simulate_WaveformDate_Period_Length >> 1)
            j++;
        else
            j--;
    }
}

// 锯齿波
static void SawtoothWaveOut(void)
{
    u16 i;
    for (i = 0; i < Simulate_WaveformDate_Period_Length; i++)
    {
        Simulation_ADC_Data[i] = i * 4096 / Simulate_WaveformDate_Period_Length;
    }
}

static void SinWaveOut(void)
{
    u16 i;
    for (i = 0; i < Simulate_WaveformDate_Period_Length; i++)
    {
        Simulation_ADC_Data[i] = (u16)(2090 + 1990 * arm_sin_f32((2 * PI * i) / Simulate_WaveformDate_Period_Length));
    }
}

void Simulate_Signal_WaveformData(u16 *SimulateWaveData)
{
    switch (Simulation_Times_Index)
    {
    case 0:
        SquareWaveOut();
        break;
    case 1:
        TriangularWaveOut();
        break;
    case 2:
        SawtoothWaveOut();
        break;
    case 3:
        SinWaveOut();
        break;
    default:
        log_debug("It is same Simulate_WaveformDate!!!");
        break;
    }
    for (u16 i = 0; i < ADC_SAMPLING_NUM / Simulate_WaveformDate_Period_Length; ++i) // 复制数据
    {
        for (u16 j = 0; j < Simulate_WaveformDate_Period_Length; ++j)
        {
            SimulateWaveData[j + i * Simulate_WaveformDate_Period_Length] = Simulation_ADC_Data[j] + Add_Noise;
        }
    }
}
