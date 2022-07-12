#include "oled_interface.h"
#include "draw_api.h"
#include "oled_config.h"
#include "stdio.h"
#include "arm_math.h"
#include "log.h"

void OLEDInterface_Display_TiGame_Logo(void)
{
    log_debug("OLEDInterface Drawing TiGame Logo...\r\n");

    DrawBitmap(0, 0, TiLOGO, 128, 64);   // Ti logo
    UpdateScreen();                      // 更新屏幕
    SelectDownOLED();                    // 选用下屏
    ClearScreen();                       // 清屏
    DrawBitmap(0, 0, GameLOGO, 128, 64); // 电赛 logo
    UpdateScreen();
    log_debug("OLEDInterface Draw Completed!\r\n");
}
// 更新屏幕

/* 显示归一化坐标轴 */
void OLEDInterface_Display_NormAm_XY(void)
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

// 找出最大值位置
u16 Compare_Max(u16 Mag[], u16 len)
{
    u16 i, Fn_Num;
    Fn_Num = 0;
    Mag[Fn_Num] = Mag[0];
    for (i = 1; i < len; i++)
    {
        if (Mag[Fn_Num] < Mag[i])
        {
            Fn_Num = i;
        }
    }
    return Fn_Num;
}

#define OLED_Interface_Waveform_Display_Y_Range 54
#define OLED_Interface_Waveform_Display_Y_Offset 5
#define OLED_Interface_Waveform_Display_Y_Calculate(DATA, MAX) \
    OLED_Y_MAX - OLED_Interface_Waveform_Display_Y_Offset - (OLED_Interface_Waveform_Display_Y_Range * DATA / MAX)

/* 计算、转换并显示波形 */
void OLEDInterface_Update_Waveform(u16 *WaveformData)
{
    u16 i = 0;
    u16 Max_Data;
    u16 Y, Y_Old;
    log_debug("OLEDInterface Updating Waveform...\r\n");
    SelectUpOLED();
    ClearScreen();
    // WaveBox();

    Max_Data = WaveformData[Compare_Max(WaveformData, OLED_X_MAX)];

    Y_Old = OLED_Interface_Waveform_Display_Y_Calculate(WaveformData[i], Max_Data);
    for (i = 1; i < OLED_X_MAX - 1; ++i)
    {
        Y = OLED_Interface_Waveform_Display_Y_Calculate(WaveformData[i], Max_Data);
        DrawLine(i, Y_Old, i + 1, Y);
        DrawPixel(i, Y + 1); // 加粗
        Y_Old = Y;
    }
    UpdateScreen();
    log_debug("OLEDInterface Update Waveform Completed!\r\n");
}

/* MinDSO-Pro示波器开源代码 */
#define CHART_H_MAX 127
#define CHART_H_MIN 0
#define CHART_V_MAX 59
#define CHART_V_MIN 4

/* 波形图背景坐标方格 */
void WaveBox(void)
{
    uint16_t i;
    SelectUpOLED();
    ClearScreen();

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

void OLEDInterface_Update_Data(float *NormalizedAm, float THD, u32 Period)
{
    uint8_t i;
    char strBuf[9];
    log_debug("OLEDInterface Updating Data...\r\n");
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
    snprintf(strBuf, 9, "T:%4dus", Period * 1000000 / TimerSourerFreq); // 测量周期
    DrawString(80, 0, strBuf);
    UpdateScreen();
    log_debug("OLEDInterface Update Data Completed!\r\n");
}
