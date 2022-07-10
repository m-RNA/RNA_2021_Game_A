/******************************************************************************
//MSP432P401R
//GPIO配置 (LED KEY BEEP OLED)
//Bilibili：m-RNA
//E-mail:m-RNA@qq.com
//创建日期:2021/8/12
*******************************************************************************/

#include "baseboard.h"
#include "delay.h"
#include "usart.h"

//函数功能：延时
static void key_delay(uint16_t t)
{
    volatile uint16_t x;
    while (t--)
        for (x = 0; x < 1000; x++)
            ;
}

uint8_t KEY_Scan(bool mode)
{
    static bool key_up = true; //按键按松开标志
    if (mode)
        key_up = true; //支持连按
    if (key_up && (KEY1_OnBoard == 0 || KEY2_OnBoard == 0 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
    {
        key_delay(25); //去抖动
        key_up = false;
        if (KEY1_OnBoard == 0)
            return KEY1_OnBoard_PRES;
        else if (KEY2_OnBoard == 0)
            return KEY2_OnBoard_PRES;
        else if (KEY1 == 0)
            return KEY1_PRES;
        else if (KEY2 == 0)
            return KEY2_PRES;
        else if (KEY3 == 0)
            return KEY3_PRES;
        else if (KEY4 == 0)
            return KEY4_PRES;
    }
    else if (KEY1_OnBoard == 1 && KEY2_OnBoard == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
        key_up = true;
    return 0; // 无按键按下
}

/************************  按键初始化函数  ***************************/

void BaseBoardInit(void)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 + GPIO_PIN4);                         // KEY_OnBoard

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0); // BEEP
    BEEP = 1;

    LED_Init();  // LED
    InitGraph(); // OLED
}

/************************  END  ****************************/
