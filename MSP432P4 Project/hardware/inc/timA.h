/****************************************************/
// MSP432P401R
// 定时器A
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/8/26
/****************************************************/

#ifndef __RNA_TIMA_H
#define __RNA_TIMA_H
#include "sysinit.h"

#define CAP_TIMA_SELECTION TIMER_A2_BASE                         //在这里改定时器
#define CAP_REGISTER_SELECTION TIMER_A_CAPTURECOMPARE_REGISTER_2 //在这里改定时器通道
#define CAP_CCR_NUM 2                                            //在这里改定时器通道
#define CAP_PORT_PIN GPIO_PORT_P5, GPIO_PIN7                     //在这里改复用引脚

extern uint8_t TIMA2_CAP_STA;
extern uint16_t true_T;

void TimA2_Cap_Init(void);
void TimA0_Int_Init(uint16_t ccr0, uint16_t psc);

#endif
