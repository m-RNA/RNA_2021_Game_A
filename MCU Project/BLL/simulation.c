#include "simulation.h"
#include "arm_math.h"
#include "log.h"
#include "stdlib.h"

#define Simulate_WaveformDate_Period_Length SignalSampleFreq_Multiple
#define Add_Noise (rand() % Simulate_Sample_ADC_Noise)
u8 Simulation_Times_Index = 0;

u32 Simulation_CCR[10] = {
    TimerSourerFreq / 1000,   // 电赛测试信号1
    TimerSourerFreq / 50000,  // 电赛测试信号2
    TimerSourerFreq / 100000, // 电赛测试信号3

    SignalSamplePeriod_MIN, // 最大采样率

    TimerSourerFreq / 30000, // 自定义
    TimerSourerFreq / 600000,
    TimerSourerFreq / 900000,
};

#define Precision 10 // 精度 - 到几次谐波
float Simulation_NormAm[10][Precision - 1] = {
    {0.00f, 0.20f, 0.00f, 0.15f}, // 电赛测试信号1
    {0.00f, 0.08f, 0.15f, 0.00f}, // 电赛测试信号2
    {0.00f, 0.00f, 0.00f, 0.10f}, // 电赛测试信号3

    {0.0f, 0.0f, 0.0f, 0.0f}, // 正弦波

    {0.00f, -0.1111111111f, 0.00f, 0.04f, 0.0f, -0.0204081633f, 0.0f, 0.0123456790f},        // 三角波
    {0.00f, 0.3333333333f, 0.0f, 0.2f, 0.0f, 0.1428571429f, 0.0f, 0.1111111111f},            // 方波
    {0.5f, 0.3333333333f, 0.25f, 0.2f, 0.1666666667f, 0.1428571429f, 0.125f, 0.1111111111f}, // 锯齿波
};

extern u16 Compare_Min(float Mag[], u16 len);
static float OWaveDate[Simulate_WaveformDate_Period_Length];
void Simulate_Signal_Synthesizer(u16 *SimulateWaveData)
{
    u16 i;
    u16 MinIndex;

    for (int i = 0; i < Simulate_WaveformDate_Period_Length; ++i)
    {
        OWaveDate[i] = arm_sin_f32(2 * PI * i / ((float)Simulate_WaveformDate_Period_Length));
        for (int j = 0; j < Precision - 1; ++j)
        {
            OWaveDate[i] += arm_sin_f32(2 * PI * (j + 2) * i / ((float)Simulate_WaveformDate_Period_Length)) * Simulation_NormAm[Simulation_Times_Index][j];
        }
    }

    // 找出最小的小数的位置
    MinIndex = Compare_Min(OWaveDate, Simulate_WaveformDate_Period_Length);
    for (i = 0; i < Simulate_WaveformDate_Period_Length; ++i)
    {
        // 将小数全转为为正数，再乘以1000变为整数
        SimulateWaveData[i] = 1000 * (OWaveDate[i] - OWaveDate[MinIndex]);
        // 这个1000是随便定的，不要太大就好了，目的是把小数转换为整数
    }

    // 复制数据
    for (u16 i = 1; i < ADC_SAMPLING_NUM / Simulate_WaveformDate_Period_Length; ++i)
    {
        for (u16 j = 0; j < Simulate_WaveformDate_Period_Length; ++j)
        {
            SimulateWaveData[j + i * Simulate_WaveformDate_Period_Length] = SimulateWaveData[j] + Add_Noise;
        }
    }
    for (u16 j = 0; j < Simulate_WaveformDate_Period_Length; ++j)
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
