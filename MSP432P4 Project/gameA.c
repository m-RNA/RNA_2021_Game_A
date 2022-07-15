/*******************************************
// MSP432P401R
// 2021年 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/11/11
*******************************************/

#include "gameA.h"
#include "draw_api.h"
#include "adc.h"
#include "timA.h"
#include "arm_math.h"
#include "math.h"
#include "stdio.h"
#include "delay.h"

uint16_t f0;            // 基波
uint16_t fx[4];         // 谐波
uint16_t waveTran[128]; // 转换显示波

extern float fft_outputbuf[ADC_SAMPLING_NUMBER * MM]; // FFT输出数组

/************************************** 相关算法 ***************************************/

// 找出最大值位置
uint16_t Compare_Max(float Mag[], uint16_t len)
{
    uint16_t i, Fn_Num;
    float Mag_max;
    Mag_max = Mag[0];
    Fn_Num = 0;
    for (i = 0; i < len; i++)
    {
        if (Mag_max < Mag[i])
        {
            Fn_Num = i;
            Mag_max = Mag[i];
        }
    }
    return Fn_Num;
}

// 找出最小值位置
uint16_t Compare_Min(float Mag[], uint16_t len)
{
    uint16_t i, Num;
    float min;
    min = Mag[0];
    Num = 0;
    for (i = 0; i < len; i++)
    {
        if (min > Mag[i])
        {
            Num = i;
            min = Mag[i];
        }
    }
    return Num;
}

/* 找出基波的所在位置 */
uint16_t FloatMax(float Mag[])
{
    uint16_t i, Fn_Num;
    float Mag_max;
    Mag_max = Mag[1];
    Fn_Num = 1;
    for (i = 1; i < ADC_SAMPLING_NUMBER / 2 * MM; i++)
    {
        if (Mag_max < Mag[i])
        {
            Fn_Num = i;
            Mag_max = Mag[i];
        }
    }
    return Fn_Num;
}

/* 找出最大值位置(带窗口) */
uint16_t FloatMax_WithWindow(float Mag[], uint16_t l, uint16_t r)
{
    uint16_t i, Fn_Num;
    float Mag_max;
    Mag_max = Mag[l];
    Fn_Num = l;
    for (i = l; i <= r; ++i)
    {
        if (Mag_max < Mag[i])
        {
            Fn_Num = i;
            Mag_max = Mag[i];
        }
    }
    return Fn_Num;
}

/* THDx计算 */
float THDx_calculate(void)
{
    float ans = 0.0f;
    float sum = 0.0f;
    uint8_t i;
    for (i = 0; i < 4; ++i) 
        sum += fft_outputbuf[fx[i]] * fft_outputbuf[fx[i]];
    ans = ceil(sqrt(sum) / fft_outputbuf[f0] * 10000) / 100.0f;
    
    /* A题THD限制 */
    if (ans > 50.0f)
        ans = 50.0f;
    else if (ans < 5.0f)
        ans = 5.0f;
    return ans;
}

/************************************** OLED显示 ***************************************/

/* 显示归一化坐标轴 */
void ShowGYH_XY(void)
{
    uint8_t i;
    SelectDownOLED();
    ClearScreen(); //清屏
    DrawString(0, 0, "THDx:00.00%");// THD
    for (i = 1; i < 5; ++i)         // 归一化幅值
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
        DrawNum(40 - 3 + 13 * i, 56, i, 1); //1 2 3 4 5
    }
    DrawLine(40 + 13, 22, 40 + 13, 52);         // 竖线 基波 1
    DrawLine(40 - 2, 22, 40 + 2, 22);           // 横线 基波 1
    DrawLine(40 + 13 - 1, 22, 40 + 13 + 1, 22); // 横线 谐波

    DrawNum(32, 20, 1, 1); // 1
}

/* 更新归一化幅值、THD */
void UpdateGYH(float *gyh, float thd)
{
    uint8_t i;
    char strBuf[9];
    SelectDownOLED();
    ClearScreen();
    ShowGYH_XY();
    for (i = 2; i < 6; ++i)
    {
        snprintf(strBuf, 5, "%1.2f", gyh[i - 2]); //归一化幅值 小数显示
        DrawString(0, 10 + 9 * (i - 1), strBuf);

        DrawLine(40 + 13 * i, 52 - 30 * gyh[i - 2], 40 + 13 * i, 52);                           // 竖线 谐波
        DrawLine(40 + 13 * i - 1, 52 - 30 * gyh[i - 2], 40 + 13 * i + 1, 52 - 30 * gyh[i - 2]); // 横线 谐波
    }
    snprintf(strBuf, 6, "%2.2f", thd); //THDx 小数显示
    DrawString(30, 0, strBuf);
    snprintf(strBuf, 9, "T:%4dus", true_T / 3); // 测量周期
    DrawString(80, 0, strBuf);
    UpdateScreen();
}

#define X_MAX 128
#define Y_MAX 64
#define Y_MULTI 54
#define Y_UPMOVE 64
#define Y_FW 62

/* 计算、转换并显示波形 */
void ShowWave_AndTran(float *gyh)
{
    uint16_t i;
    uint16_t minNum;
    uint16_t maxNum;
    uint16_t vm;
    uint16_t temp_last, temp;
    float waveDate[X_MAX];
    for (int i = 0; i < X_MAX; ++i)
    {
        waveDate[i] = Y_MULTI * arm_sin_f32(PI * i / 64.0f);
        for (int j = 0; j < 4; ++j)
        {
            waveDate[i] += Y_MULTI * arm_sin_f32(PI * i * (j + 2) / 64.0f) * gyh[j];
        }
        waveDate[i] += Y_UPMOVE;
        //printf("%f\n", waveDate[i]);
    }

    minNum = Compare_Min(waveDate, X_MAX);
    maxNum = Compare_Max(waveDate, X_MAX);
    vm = waveDate[maxNum] - waveDate[minNum];
    SelectUpOLED();
    ClearScreen();
    //WaveBox();
    waveTran[0] = waveDate[0] - waveDate[minNum];
    DrawPixel(i, temp_last);
    waveTran[1] = waveDate[1] - waveDate[minNum];
    temp_last = (uint16_t)(64 - waveTran[1] * 54 / (float)vm - 5);
    for (i = 2; i < X_MAX; ++i)
    {
        waveTran[i] = waveDate[i] - waveDate[minNum];
        temp = (uint16_t)(64 - waveTran[i] * 54 / (float)vm - 5);
        //printf("%d,", waveTran[i]);
        DrawLine(i, temp_last, i + 1, temp);
        //printf("%d\r\n", temp);
        // 加粗
        DrawPixel(i + 1, temp);
        temp_last = temp;
    }
    UpdateScreen();
}

/* OLED_LIB 开源代码 */
void RoundClock(int hours, int minute, int sec)
{
    unsigned char i = 0;
    TypeXY hourspoint, minutepoint, secpoint, tmp1, tmp2;
    //时针
    SetRotateValue(63, 31, hours * 30 + (minute * 30) / 60, 1);
    hourspoint = GetRotateXY(63 - 14, 31);
    DrawLine(63, 31, hourspoint.x, hourspoint.y);
    //分针
    SetRotateValue(63, 31, minute * 6 + (sec * 6) / 60, 1);
    minutepoint = GetRotateXY(63 - 21, 31);
    DrawLine(63, 31, minutepoint.x, minutepoint.y);
    //秒针
    SetRotateValue(63, 31, sec * 6, 1);
    secpoint = GetRotateXY(63 - 28, 31);
    DrawLine(63, 31, secpoint.x, secpoint.y);
    //表盘
    for (i = 0; i < 12; i++)
    {
        SetRotateValue(63, 31, i * 30, 1);
        tmp1 = GetRotateXY(63 - 29, 31);
        tmp2 = GetRotateXY(63 - 24, 31);
        DrawLine(tmp1.x, tmp1.y, tmp2.x, tmp2.y);
    }
    DrawFillCircle(63, 31, 2);
    DrawCircle(63, 31, 30);
    UpdateScreen();
    ClearScreen();
}

/* 等待动画 */
void WaitingAnimat(uint16_t a)
{
    uint16_t j, z;
    SelectUpOLED();
    ClearScreen();
    DrawBitmap(0, 0, Measuring, 128, 64); // 显示测量中
    UpdateScreen();
    SelectDownOLED();
    ClearScreen();
    for (j = 0; j < a; j++)
        for (z = 0; z < 60; z++)
        {
            RoundClock(0, j, z);
            delay_ms(3);
        }
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
    //DrawFastVLine(CHART_H_MIN , CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);
    //DrawFastVLine(CHART_H_MAX , CHART_V_MIN, CHART_V_MAX - CHART_V_MIN + 1);
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
