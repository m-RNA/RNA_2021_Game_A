#include "oled_interface.h"
#include "draw_api.h"
#include "oled_config.h"
#include "stdio.h"
#include "my_math.h"
#include "log.h"

// OLED 波形显示区域坐标和范围定义
#define OLEDInterface_Waveform_Display_Y_Range 54
#define OLEDInterface_Waveform_Display_Y_Offset 5

#if 0
/* 波形图背景坐标方格（没用上） */
static void WaveBox(void)
{
#define CHART_H_MIN 0
#define CHART_H_MAX OLED_X_MAX
#define CHART_V_MIN OLEDInterface_Waveform_Display_Y_Offset
#define CHART_V_MAX (OLEDInterface_Waveform_Display_Y_Range + OLEDInterface_Waveform_Display_Y_Offset)
    uint16_t i;
    SelectUpOLED();
    ClearScreen();
    /* MinDSO-Pro示波器开源代码移植 */
    DrawFastHLine(CHART_H_MIN, CHART_V_MIN, 4);
    DrawFastHLine(CHART_H_MIN, CHART_V_MAX, 4);
    DrawFastHLine(CHART_H_MAX - 3, CHART_V_MIN, 4);
    DrawFastHLine(CHART_H_MAX - 3, CHART_V_MAX, 4);
    DrawFastHLine(CHART_H_MIN + 25 - 2, CHART_V_MIN, 5);
    DrawFastHLine(CHART_H_MIN + 25 - 2, CHART_V_MAX, 5);
    DrawFastHLine(CHART_H_MIN + 50 - 2, CHART_V_MIN, 5);
    DrawFastHLine(CHART_H_MIN + 50 - 2, CHART_V_MAX, 5);
    DrawFastHLine(CHART_H_MIN + 75 - 2, CHART_V_MIN, 5);
    DrawFastHLine(CHART_H_MIN + 75 - 2, CHART_V_MAX, 5);
    for (i = 1; i < 5; i++)
    {
        DrawFastHLine(CHART_H_MIN + 25 * i - 2, CHART_V_MIN, 5);
        DrawFastHLine(CHART_H_MIN + 25 * i - 2, CHART_V_MAX, 5);
    }
    // DrawFastVLine(CHART_H_MIN , CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);
    // DrawFastVLine(CHART_H_MAX , CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);
    for (i = 0; i < 18; i++)
    {
        DrawFastHLine(CHART_H_MIN + 7 * i, CHART_V_MIN + ((CHART_V_MAX - CHART_V_MIN) >> 1), 3);
    }
    for (i = 0; i < 7; i++)
    {
        DrawFastVLine(CHART_H_MIN + 25, CHART_V_MIN + 1 + i * 8, 3);
        DrawFastVLine(CHART_H_MIN + 50, CHART_V_MIN + 1 + i * 8, 3);
        DrawFastVLine(CHART_H_MIN + 75, CHART_V_MIN + 1 + i * 8, 3);
        DrawFastVLine(CHART_H_MIN + 100, CHART_V_MIN + 1 + i * 8, 3);
    }
    UpdateScreen();
}
#endif

/* 显示归一化幅值图表坐标系 */
static void OLEDInterface_Display_NormAm_XY(void)
{
    uint8_t i;
    SelectDownOLED();
    ClearScreen();                   //清屏
    DrawString(0, 0, "THDx:00.00%"); // THD
    for (i = 1; i < 5; ++i)          // 归一化幅值
    {
        DrawString(0, 10 + 9 * i, "0.00");
    }
    DrawNum(32, 56, 0, 1);              // 0
    DrawLine(30, 52, 120, 52);          // x轴
    DrawLine(40, 12, 40, 62);           // y轴
    DrawLine(120 - 2, 52 - 2, 120, 52); // x轴箭头
    DrawLine(120 - 2, 52 + 2, 120, 52); // x轴箭头
    DrawLine(40 + 2, 12 + 2, 40, 12);   // y轴箭头
    DrawLine(40 - 2, 12 + 2, 40, 12);   // y轴箭头
    for (i = 1; i < 6; ++i)
    {
        DrawNum(40 - 3 + 13 * i, 56, i, 1); // 1 2 3 4 5
    }
    DrawLine(40 + 13, 22, 40 + 13, 52);         // 竖线 基波 1
    DrawLine(40 - 2, 22, 40 + 2, 22);           // 横线 基波 1
    DrawLine(40 + 13 - 1, 22, 40 + 13 + 1, 22); // 横线 谐波

    DrawNum(32, 20, 1, 1); // 1
}

/* OLED显示波形更新*/
void OLEDInterface_Update_Waveform(u16 *WaveformData)
{
    u16 i = 0;
    u16 Max_Data;
    u16 Y, Y_Old;
    log_detail("OLEDInterface Updating Waveform...\r\n");
    SelectUpOLED();
    ClearScreen();
    // WaveBox();

    Max_Data = WaveformData[Max_Unsigned(WaveformData, OLED_X_MAX)];

#define OLEDInterface_Waveform_Display_Y_Calculate(DATA, MAX) \
    (OLED_Y_MAX - OLEDInterface_Waveform_Display_Y_Offset -   \
     (OLEDInterface_Waveform_Display_Y_Range * DATA / MAX))

    Y_Old = OLEDInterface_Waveform_Display_Y_Calculate(WaveformData[i], Max_Data);
    for (i = 1; i < OLED_X_MAX - 1; ++i)
    {
        Y = OLEDInterface_Waveform_Display_Y_Calculate(WaveformData[i], Max_Data);
        DrawLine(i, Y_Old, i + 1, Y);
        DrawPixel(i, Y + 1); // 加粗
        Y_Old = Y;
    }
    UpdateScreen();
    log_detail("OLEDInterface Update Waveform Completed!\r\n");
}

/* OLED显示信息更新 */
void OLEDInterface_Update_Data(float *NormalizedAm, float THD, u32 Period)
{
    uint8_t i;
    char strBuf[9];
    log_detail("OLEDInterface Updating Data...\r\n");
    SelectDownOLED();
    ClearScreen();
    OLEDInterface_Display_NormAm_XY();
    for (i = 2; i < 6; ++i)
    {
        snprintf(strBuf, 5, "%1.2f", NormalizedAm[i - 2]); //归一化幅值 小数显示
        DrawString(0, 10 + 9 * (i - 1), strBuf);

        DrawLine(40 + 13 * i, 52 - 30 * NormalizedAm[i - 2], 40 + 13 * i, 52);                                    // 竖线 谐波
        DrawLine(40 + 13 * i - 1, 52 - 30 * NormalizedAm[i - 2], 40 + 13 * i + 1, 52 - 30 * NormalizedAm[i - 2]); // 横线 谐波
    }
    snprintf(strBuf, 6, "%2.2f", THD); // THDx 小数显示
    DrawString(30, 0, strBuf);
    snprintf(strBuf, 9, "T:%4dus", Period * 1000000 / TIMER_SOURER_FREQ); // 测量周期
    DrawString(80, 0, strBuf);
    UpdateScreen();
    log_detail("OLEDInterface Update Data Completed!\r\n");
}

/* 显示 Ti和电赛 Logo */
void OLEDInterface_Display_TiGame_Logo(void)
{
    log_detail("OLEDInterface Drawing TiGame Logo...\r\n");
    SelectUpOLED();                      // 选用上屏
    DrawBitmap(0, 0, TiLOGO, 128, 64);   // Ti logo
    UpdateScreen();                      // 更新屏幕
    SelectDownOLED();                    // 选用下屏
    ClearScreen();                       // 清屏
    DrawBitmap(0, 0, GameLOGO, 128, 64); // 电赛 logo
    UpdateScreen();                      // 更新屏幕
    log_detail("OLEDInterface Draw Completed!\r\n");
}
