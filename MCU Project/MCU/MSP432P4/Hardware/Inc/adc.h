/****************************************************/
//MSP432P401R
//ADC采集 + DMA传输
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/11/12
/****************************************************/

#ifndef __ADC_H
#define __ADC_H
#include "sysinit.h"

// 宏定义 @BiliBili：物联世界
#define ADC_TRIGGER_TA0_C1 ADC_TRIGGER_SOURCE1
#define ADC_TRIGGER_TA0_C2 ADC_TRIGGER_SOURCE2
#define ADC_TRIGGER_TA1_C1 ADC_TRIGGER_SOURCE3
#define ADC_TRIGGER_TA1_C2 ADC_TRIGGER_SOURCE4
#define ADC_TRIGGER_TA2_C1 ADC_TRIGGER_SOURCE5
#define ADC_TRIGGER_TA2_C2 ADC_TRIGGER_SOURCE6
#define ADC_TRIGGER_TA3_C1 ADC_TRIGGER_SOURCE7

void ADC_Config(void);
void adc_dma_init(u16 *Data, uint16_t Length);

#endif
