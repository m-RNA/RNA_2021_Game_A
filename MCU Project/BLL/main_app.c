#include "config.h"
#include "bsp.h"
#include "bll.h"
#include "bll_oled.h"
#include "log.h"

u32 Signal_Captured_Value; // 滞回比较器 定时器捕获值
u16 Signal_ADC_Data[ADC_SAMPLING_NUM]; // 信号采样ADC数据
u16 WaveformData[128] = {0}; // 还原平滑波形数据

float THDx = 0.0f; // 失真度测量值
float NormalizedAm[4] = {0}; // 归一化幅值：2-5次谐波
float FFT_Output[ADC_SAMPLING_NUM]; // FFT计算输出

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    
    BSP_LED_KEY_BEEP_Init();
    BSP_Uart_PC_Init(); // 第7讲 串口配置（调试）
    // InitGraph(); // OLED
    // OLEDInterface_Display_TiGame_Logo(); //  显示 Ti和电赛 Logo 

    BSP_Uart_Bluetooth_Init(); // 第7讲 串口配置 （蓝牙）
    BSP_Sample_ADC_with_DMA_Init(Signal_ADC_Data, ADC_SAMPLING_NUM);    // 第11讲 ADC 第12讲 DMA
    BSP_Sample_Timer_Init(); // 第8讲 定时器配置 （ADC触发时钟源 fs）（过零比较器采频率）
    
    /* 初始化完毕 可以测量 */
    log_debug("All Init Completed!\r\n");
    while (1)
    {
        Signal_F0_Measure(&Signal_Captured_Value); // 测量f 黄灯
        Signal_Fs_Adjust(Signal_Captured_Value);   // 调整fs 红灯
        SignalSample_Start(Signal_ADC_Data);       // 开启ADC采集DMA传输 关灯
        
        SignalSample_FFT_to_Am(Signal_ADC_Data, FFT_Output); // 通过FFT 计算各个频率分量幅值 白灯
        NormalizedAm_And_CalculateTHD(FFT_Output, NormalizedAm, &THDx); // 归一化幅值 和 计算THDx 绿色
        
        // OLEDInterface_Update_Data(NormalizedAm, THDx, Signal_Captured_Value); // 更新OLED上的信息 青色
        Transform_NormalizedAm_To_WaveformData(NormalizedAm, WaveformData); // 将归一化幅值转化为波形数据（长度内定为OLED的X分辨率128） 品红
        // OLEDInterface_Update_Waveform(WaveformData); // 将波形数据传入 让OLED在对应位置画出波形 单红
        
        Bluetooth_SendDate_To_Phone(NormalizedAm, THDx, WaveformData); // 将数据通过蓝牙发至手机 蓝色

        delay_ms(100); //延时100ms
    }
}

