#ifndef __BLL_OLED_H
#define __BLL_OLED_H
#include "sysinit.h"
#include "config.h"

void OLEDInterface_Display_TiGame_Logo(void);
void OLEDInterface_Update_Data(float *NormalizedAm, float THD, u32 Period);
void OLEDInterface_Update_Waveform(u16 *WaveformData);



#endif
