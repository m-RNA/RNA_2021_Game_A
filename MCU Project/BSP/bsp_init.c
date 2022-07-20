#include "bsp_init.h"
#include "bsp_operation.h"
#include "bsp_it.h"
#include "log.h"

/********************************************************************************************/
/******************************   初始化类函数  *********************************************/

void BSP_GPIO_Init(void)
{
#ifdef __MSP432P401R__
    LED_Init(); // LED
#else
    MX_GPIO_Init();
#endif
}

void BSP_OLEDInterface_Init(void)
{
    log_detail("OLED Interface Init...\r\n");
    InitGraph();
}

void BSP_Uart_PC_Init(void)
{
#ifdef __MSP432P401R__
    uart_init(1382400); // 第7讲 串口配置（调试）
#else
    MX_USART1_UART_Init(); // 第7讲 串口配置（调试）
#endif
    log_detail("Uart PC Init Completed!\r\n");
}

void BSP_Uart_Bluetooth_Init(void)
{
    log_detail("Uart Bluetooth Init...\r\n");
#ifdef __MSP432P401R__
    usart3_init(9600);
#else
    MX_USART2_UART_Init();
#endif
}

void BSP_Sample_Timer_Init(void)
{
    log_detail("Sample Timer Init...\r\n");
#ifdef __MSP432P401R__
    TimA2_Cap_Init(TIMER_A_CLOCKSOURCE_DIVIDER_1);                              // 第8讲 定时器捕获 （过零比较器采频率）
    TimA0_Base_Init(TIMER_SOURER_FREQ / 500000, TIMER_A_CLOCKSOURCE_DIVIDER_1); // 第8讲 定时器配置 （ADC触发时钟源 fs）
#else
    MX_TIM2_Init(); // 第8讲 定时器捕获 （过零比较器采频率）
    MX_TIM3_Init(); // 第8讲 定时器配置 （ADC触发时钟源 fs）
#endif
    BSP_Timer_Start(Signal_Capture_Timer);
    BSP_Timer_Start(Signal_Sample_Timer);
}

void BSP_Sample_ADC_with_DMA_Init(u16 *Addr, u16 Length)
{
    log_detail("Sample ADC And DMA Init...\r\n");
#ifdef __MSP432P401R__
    adc_dma_init(Addr, Length); // 第12讲 DMA
    ADC_Config();               // 第11讲 ADC
#else
    extern void DMA_ADC_Transmit_Complete_Callback(DMA_HandleTypeDef * _hdma);

    MX_DMA_Init();
    HAL_DMA_RegisterCallback(DMA_ADC, HAL_DMA_XFER_CPLT_CB_ID, DMA_ADC_Transmit_Complete_Callback);
    MX_ADC1_Init();
    HAL_ADC_Start_DMA(SIGNAL_SAMPLE_ADC, (u32 *)Addr, Length);
#endif
}
