/****************************************************/
//MSP432P401R
//ADC采集 + DMA传输
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/11/12
/****************************************************/

#include "adc.h"
#include "usart.h"
#include "timA.h"
#include "led.h"
#include "delay.h"

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment = 1024
#elif defined(__GNUC__)
__attribute__((aligned(1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[32];

volatile bool recv_done_flag = 0;
uint16_t adc_inputbuf[MM][ADC_SAMPLING_NUMBER]; //FFT输入数组

// DMA初始化函数
void adc_dma_init(uint16_t len)
{
  /* Configuring DMA module */
  MAP_DMA_enableModule();
  MAP_DMA_setControlBase(MSP_EXP432P401RLP_DMAControlTable);
  //	DMA_disableChannelAttribute(DMA_CH7_ADC14, UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
  MAP_DMA_assignChannel(DMA_CH7_ADC14);
  MAP_DMA_setChannelControl(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1);
  MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)adc_inputbuf, len);
  MAP_DMA_assignInterrupt(INT_DMA_INT1, 7);
  MAP_DMA_clearInterruptFlag(DMA_CH7_ADC14 & 0x0F);
  MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
  MAP_DMA_enableInterrupt(INT_DMA_INT1);
  MAP_DMA_enableChannel(7);
}

/*****************   配置说明   *****************
 *
 * 以上DMA的配置代码，
 * 源码为BiliBili平台UP主 “CloudBoyStudio” 编写
 * 本人RNA，不是作者
 * 在此也表感谢
 *
 *****************   说明结束   *****************/

// 这里时尝试不开乒乓模式 采集更多的点数 但是时间来不及了 于是做一半
void DMA_INT1_IRQHandler(void)
{
  static uint8_t i = 0;
  ADC14_clearInterruptFlag(ADC_INT0);
  DMA_clearInterruptFlag(7);
  LED_B ^= 1;
  ++i;
  if (i != MM) 
  {
    DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)&adc_inputbuf[i][0], 1024);
    DMA_enableChannel(7);
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); //开启计数
    return;
  }
  else
  {
    Timer_A_stopTimer(TIMER_A0_BASE);
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
    ADC14_clearInterruptFlag(ADC_INT0);

    recv_done_flag = 1;
    i = 0;
  }

  // DMA_disableChannel(7);	// dma will auto disable channel if complete

  /* Disable the interrupt to allow execution */
  // Interrupt_disableInterrupt(INT_DMA_INT1);
  // DMA_disableInterrupt(INT_DMA_INT1);
}

// ADC初始化函数
void ADC_Config(void)
{
  /* Initializing ADC (MCLK/1/1) */
  MAP_ADC14_enableModule();                                                                 // 使能ADC14模块
  MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, ADC_NOROUTE); // 初始化ADC时钟 超频至48MHz

  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN6, GPIO_TERTIARY_MODULE_FUNCTION); // 模拟输入
  MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);                                                    // 单通道配置 多次转化true
  MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A7, false);     // 使用内部2.5V电压参考 非差分输入false
  MAP_ADC14_enableInterrupt(ADC_INT0);                                                                    // ADC通道0的中断

  MAP_REF_A_setReferenceVoltage(REF_A_VREF2_5V); // 使用内部2.5V电压参考
  MAP_REF_A_enableReferenceVoltage();            // 使能内部2.5V电压参考

  MAP_ADC14_setSampleHoldTrigger(ADC_TRIGGER_TA0_C1, false);  // 使用定时器A通道1作为触发源
    
  MAP_ADC14_enableConversion(); // 使能开始转换(触发后 自动ADC上电)
  delay_ms(1000);               // 等待稳定
}
