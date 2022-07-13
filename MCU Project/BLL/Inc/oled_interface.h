#ifndef __BLL_OLED_H
#define __BLL_OLED_H
#include "config.h"

// OLED XY最值
#define OLED_X_MAX SCREEN_COLUMN
#define OLED_Y_MAX SCREEN_ROW

/* 显示 Ti和电赛 Logo */
void OLEDInterface_Display_TiGame_Logo(void);

/* OLED显示信息更新 */
void OLEDInterface_Update_Data(float *NormalizedAm, float THD, u32 Period);

/* OLED显示波形更新*/
void OLEDInterface_Update_Waveform(u16 *WaveformData);

#endif
