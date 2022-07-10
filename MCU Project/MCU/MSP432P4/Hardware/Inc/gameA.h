/*******************************************
// MSP432P401R
// 2021年 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/11/11
*******************************************/

#ifndef __RNA_2021_GAME_A_H
#define __RNA_2021_GAME_A_H
#include "sysinit.h"

// xx倍数... 队友的理论
#define FDBS 4

extern uint16_t f0;            // 基波
extern uint16_t fx[4];         // 谐波
extern uint16_t waveTran[128]; // 转换显示波

/* 找出基波的所在位置 */
uint16_t FloatMax(float Mag[]);

/* 找出最大值位置(带窗口) */
uint16_t FloatMax_WithWindow(float Mag[], uint16_t l, uint16_t r);

/* THDx计算 */
float THDx_calculate(void);

/* 显示归一化坐标轴 */
void ShowGYH_XY(void);

/* 更新归一化幅值、THD */
void UpdateGYH(float *gyh, float thd);

/* 计算、转换并显示波形 */
void ShowWave_AndTran(float *gyh);

/* 等待动画 */
void WaitingAnimat(uint16_t a);

/* 波形图背景坐标方格显示 */
void WaveBox(void);

#endif
