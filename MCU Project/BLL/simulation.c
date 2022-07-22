#include "simulation.h"
#include "arm_math.h"
#include "my_math.h"
#include "log.h"
#include "stdlib.h"

#define Simulate_WaveformDate_Period_Length SIGNAL_SAMPLE_FREQ_MULTIPLE
#define ADD_NOISE (rand() % SIMULATE_SAMPLE_ADC_NOISE)
u8 Simulation_Times_Index = 0;

#define SYNTHESIZE_PRECISION 5 // 精度 - 到几次谐波
u16 Simulation_Fx_Vpp_Data[SIMULATION_TIMES][SYNTHESIZE_PRECISION] = {
    {400, 0, 80, 0, 60}, // 电赛测试信号1 THDo = 25.0%
    {200, 0, 16, 30, 0}, // 电赛测试信号2 THDo = 17.0%
    {30, 0, 0, 0, 3},    // 电赛测试信号3 THDo = 10.0%

    {30, 0, 0, 3, 0}, // 自定义 THDo = 10.0%
    {30, 0, 3, 0, 0}, // 自定义 THDo = 10.0%
    {30, 3, 0, 0, 0}, // 自定义 THDo = 10.0%
    {10, 0, 0, 0, 1}, // 自定义 THDo = 10.0%
};

u32 Simulation_CCR_Data[SIMULATION_TIMES] = {
    TIMER_SOURER_FREQ / 1000,   // 电赛测试信号1
    TIMER_SOURER_FREQ / 50000,  // 电赛测试信号2
    TIMER_SOURER_FREQ / 100000, // 电赛测试信号3

    0xFFFF,                     // 最小采样率
    SIGNAL_SAMPLE_PERIOD_MIN,   // 最大采样率
    TIMER_SOURER_FREQ / 300000, // 自定义
    TIMER_SOURER_FREQ / 600000,
};

float Simulation_Phase_Data[SIMULATION_TIMES][SYNTHESIZE_PRECISION] = {
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f}, // 电赛测试信号1
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f}, // 电赛测试信号2
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f}, // 电赛测试信号3

    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f}, // 自定义
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
    {0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
};

u16 Simulate_Fs_ARR = 0;
void Simulation_Set_Fs_ARR(u16 Fs_ARR)
{
    Simulate_Fs_ARR = Fs_ARR;
}

void Simulate_Signal_Synthesizer(u16 *SimulateWaveData, u16 Length)
{
    u16 i, j;
    u16 Data_Index;
    u16 Freq_Multiple;
    // u16 MinIndex;
    extern u8 OverSamplingFlag;
    if (!OverSamplingFlag)
    {
        Freq_Multiple = Simulation_CCR_Data[Simulation_Times_Index] / Simulate_Fs_ARR;
    }
    else
    {
        Freq_Multiple = (SIGNAL_SAMPLE_FREQ_MULTIPLE + 1) * Simulation_CCR_Data[Simulation_Times_Index] / Simulate_Fs_ARR;
    }
    if (Freq_Multiple >= Signal_Synthesizer_Wave_Length_MAX)
        log_assert("\r\n\
    Simulated ERROR: Freq_Multiple is too Big! MayBe:\r\n\
    1. ADC_SAMPLING_NUM    is Too Large.\r\n\
    2. SIMULATION_CCR_MAX  is Too Small.\r\n\
    3. Simulation_CCR_Data is Too Large(Setting Frequency is too Low).\r\n");

    Signal_Synthesizer_Vpp(SimulateWaveData, Freq_Multiple, Simulation_Fx_Vpp_Data[Simulation_Times_Index],
                           Simulation_Phase_Data[Simulation_Times_Index], SYNTHESIZE_PRECISION);

    // 复制数据
    for (i = 1; i <= Length / Freq_Multiple; ++i)
    {
        for (j = 0; j < Freq_Multiple; ++j)
        {
            Data_Index = j + i * Freq_Multiple;
            // MSP432 的ADC为14位 同时选用内部2.5V参考电压源
            SimulateWaveData[Data_Index] = (SimulateWaveData[j] + ADD_NOISE) * ADC_MAX / ADC_RF_V_MV;
            if (Data_Index >= Length)
                break;
        }
    }
    for (j = 0; j < Freq_Multiple; ++j)
    {
        // MSP432 的ADC为14位 同时选用内部2.5V参考电压源
        SimulateWaveData[j] = (SimulateWaveData[j] + ADD_NOISE) * ADC_MAX / ADC_RF_V_MV;
    }
}

/*******************************************************************/
#if 0
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
            SimulateWaveData[j + i * Simulate_WaveformDate_Period_Length] = Simulation_ADC_Data[j] + ADD_NOISE;
        }
    }
}
#endif
