/****************************************************/
// MSP432P401R
// 定时器A
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/8/26
/****************************************************/

#ifndef __RNA_TIMA_H
#define __RNA_TIMA_H
#include "config.h"

void TimA2_Cap_Init(uint16_t psc);
void TimA0_Base_Init(uint16_t ccr0, uint16_t psc);

#endif
