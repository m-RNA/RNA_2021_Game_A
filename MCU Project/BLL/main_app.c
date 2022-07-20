/*******************************************
// 2021 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2022/07/11
*******************************************/

/******************************************
 * 使用工程提示：Stm32工程支持Keil仿真
 *
 * ADC采样点数、采样频率倍数、仿真信号输入等等
 * 可去"config.h"头文件配置
 *
 * *****************************************
 *
 * 默认自带仿真输入信号
 * 即 ->无需<- ADC前端电路 信号发生器输入信号
 * 也可测试算法
 * 更多仿真输入信号配置
 * 可去"simulation.h"和"simulation.c"文件配置
 *
 * *****************************************
 *
 * 默认调试器串口会打印大量信息
 * 可去"config.h"修改宏定义
 * DEBUG_PRINT_INTERNAL_DATA
 * 为0，来关闭打印大量内部信息
 *
 * 信息的查看可以使用纸飞机串口调试助手
 * ascii协议通过log_data_to_draw_ascii
 * stamp协议通过log_data_to_draw_stamp
 * 或者可以使用断点调试，将信息复制后，粘贴到
 * /RNA_2021_Game_A/Doc/AnalyzeData 表格里
 * 有多个分表，ADC采样点数不同请粘贴到对应位置
 *
 * ******************************************/

#include "config.h"
#include "bsp_init.h"
#include "bll.h"
#include "log.h"
#include "my_math.h"
#include "oled_interface.h"

u32 Signal_Captured_Value;                   // 滞回比较器 定时器捕获值
u16 Signal_ADC_Data[ADC_SAMPLING_NUM];       // 信号采样ADC数据
u16 WaveformData_Restored[OLED_X_MAX] = {0}; // 还原平滑波形数据

float THDx = 0.0f;           // 失真度测量值
float NormalizedAm[4] = {0}; // 归一化幅值：2-5次谐波
float Phase[5] = {0};        // 各分量相位（占位，还没用上）

u16 Fx_Vpp[5] = {0};                    // 幅值(uV)
float Amplitude_Data[ADC_SAMPLING_NUM]; // 各个频率分量幅值(FFT后)

int main(void)
{
    /***  ↑↑↑ 使用工程提示在上面 ↑↑↑  ***/
    /***  ↑↑↑     请仔细阅读     ↑↑↑  ***/
    /***  ↑↑↑ 使用工程提示在上面 ↑↑↑  ***/
    u16 i = 1;
    HAL_Init();
    SystemClock_Config();

    BSP_GPIO_Init();    // 第2讲 GPIO配置
    BSP_Uart_PC_Init(); // 第7讲 串口配置（调试）

    BSP_OLEDInterface_Init();            // 第10讲 OLED显示
    OLEDInterface_Display_TiGame_Logo(); // 显示 Ti和电赛 Logo

    BSP_Uart_Bluetooth_Init(); // 第7讲 串口配置 （蓝牙）

    BSP_Sample_ADC_with_DMA_Init(Signal_ADC_Data, ADC_SAMPLING_NUM); // 第11讲 ADC 第12讲 DMA
    BSP_Sample_Timer_Init();                                         // 第8讲 定时器配置 （ADC触发时钟源 fs）（过零比较器采频率）

    /* 初始化完毕 可以测量 */
    log_detail("All Init Completed!\r\n");
    log_debug("\r\n\r\n***********************  000  ****************************\r\n\r\n");

    while (1)
    {
        Signal_F0_Measure(&Signal_Captured_Value); // 测量f 黄灯
        Signal_Fs_Adjust(Signal_Captured_Value);   // 调整fs(判断是否需要等效采样) 红灯
        SignalSample_Start(Signal_ADC_Data);       // 开启ADC采集DMA传输 关灯

        CalculateAmplitude_By_FFT(Amplitude_Data, Signal_ADC_Data);                        // 通过FFT 计算各个频率分量幅值 白灯
        NormalizedAm_And_CalculateTHD(Phase, NormalizedAm, Fx_Vpp, &THDx, Amplitude_Data); // 归一化幅值 计算各分量相位 计算THDx 绿色
        Restore_Waveform_By_Vpp(WaveformData_Restored, Fx_Vpp, Phase);                     // 用幅值+各分量相位 还原波形（长度内定为OLED的X分辨率128） 品红

        OLEDInterface_Update_Data(NormalizedAm, THDx, Signal_Captured_Value); // OLED显示信息更新 青色
        OLEDInterface_Update_Waveform(WaveformData_Restored);                 // OLED显示波形更新 单红

        Bluetooth_SendDate_To_Phone(NormalizedAm, THDx, WaveformData_Restored); // 将数据通过蓝牙发至手机 蓝色

        log_data_to_draw_ascii(Signal_ADC_Data, Amplitude_Data,
                               WaveformData_Restored, NormalizedAm, Phase,
                               THDx, Signal_Captured_Value); // 打印内部数据

        log_debug("***********************  0%u0  ****************************\r\n", i++);

#if !Simulation
        delay_ms(100); //延时100ms
    }
}
#else
        if (++Simulation_Times_Index >= SIMULATION_TIMES)
        {
            Simulation_Times_Index = 0;
            log_debug("Simulate Loop!\r\n");
        }
    }
}
#endif
