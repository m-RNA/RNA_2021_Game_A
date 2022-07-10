/*******************************************
// MSP432P401R
// 2021年 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/11/11
*******************************************/

#include "sysinit.h"
#include "usart.h"
#include "led.h"
#include "timA.h"
#include "delay.h"
#include "baseboard.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "adc.h"
#include "usart3.h"
#include "gameA.h"
#include "bll.h"
#include "bll_oled.h"
#include "config.h"
#include "log.h"

int main(void)
{
    u32 Signal_Captured_Value; // 滞回比较器 定时器捕获值
    u16 Signal_ADC_Data[ADC_SAMPLING_NUM]; // 信号采样ADC数据
    float FFT_Output[ADC_SAMPLING_NUM]; // FFT计算输出
    float NormalizedAm[4] = {0}; // 归一化幅值：2-5次谐波
    float THDx = 0.0f; // 失真度测量值
    u16 WaveformData[128] = {0};
    
    MAP_Interrupt_disableMaster(); // 关闭总中断
    
    /***   三大初始化函数   ***/
    SysInit();          // 第3讲 时钟配置（48M）
    delay_init();       // 第4讲 滴答延时
    uart_init(1382400); // 第7讲 串口配置 （调试）
    
    InitGraph(); // OLED
    LED_Init();  // LED

    /**  显示 Ti和电赛 Logo  **/
    OLEDInterface_Display_TiGame_Logo();

    usart3_init(9600);     // 第7讲 串口配置 （蓝牙）
    TimA0_Int_Init(60, 1); // 第8讲 定时器配置 （ADC触发时钟源 fs）
    TimA2_Cap_Init();      // 第8讲 定时器捕获 （过零比较器采频率）
    adc_dma_init(Signal_ADC_Data, ADC_SAMPLING_NUM);    // 第12讲 DMA
    ADC_Config();          // 第11讲 ADC

    /* 初始化完毕 可以测量 */
    log_debug("All Init Completed!\r\n");
    
    MAP_Interrupt_enableMaster(); // 开启总中断
    while (1)
    {
        Signal_F0_Measure(&Signal_Captured_Value); // 测量f 黄灯
        Signal_Fs_Adjust(Signal_Captured_Value);   // 调整fs 红灯
        SignalSample_Start(Signal_ADC_Data);       // 开启ADC采集DMA传输 关灯
        
        SignalSample_FFT_to_Am(Signal_ADC_Data, FFT_Output); // 通过FFT 计算各个频率分量幅值 白灯
        NormalizedAm_And_CalculateTHD(FFT_Output, NormalizedAm, &THDx); // 归一化幅值 和 计算THDx 绿色
        
        OLEDInterface_Update_Data(NormalizedAm, THDx, Signal_Captured_Value); // 更新OLED上的信息 青色
        Transform_NormalizedAm_To_WaveformData(NormalizedAm, WaveformData); // 将归一化幅值转化为波形数据（长度内定为OLED的X分辨率128） 品红
        OLEDInterface_Update_Waveform(WaveformData); // 将波形数据传入 让OLED在对应位置画出波形 单红
        
        Bluetooth_SendDate_To_Phone(NormalizedAm, THDx, WaveformData); // 将数据通过蓝牙发至手机 蓝色

        delay_ms(100); //延时100ms
    }
}
