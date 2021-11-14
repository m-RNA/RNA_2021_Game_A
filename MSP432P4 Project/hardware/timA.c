/****************************************************/
// MSP432P401R
// 定时器A
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/8/26
/****************************************************/

#include "timA.h"
#include "usart.h"
#include "adc.h"
#include "led.h"
#include "usart3.h"

/*****************************************************************************************************************/
/*******************************             ADC TIMA0                 *******************************************/

void TimA0_Int_Init(uint16_t ccr0, uint16_t psc)
{
    const Timer_A_UpModeConfig upConfig =
        {
            TIMER_A_CLOCKSOURCE_SMCLK, // smCLK Clock Source
            psc,                       // smCLK/48 = 1Mhz
            ccr0,
            TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
            TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
            TIMER_A_DO_CLEAR                     // Clear Counter
        };
    const Timer_A_CompareModeConfig compareConfig =
        {
            TIMER_A_CAPTURECOMPARE_REGISTER_1,        // Use CCR1
            TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // Disable CCR interrupt
            TIMER_A_OUTPUTMODE_SET_RESET,             // Toggle output but
            24                                        //  Period
        };
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig); //选择定时器A1 TIMER_A0
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &compareConfig);

    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); //开启计数
}

/*****************************************************************************************************************/
/******************************************     CAP TIMA2          ***********************************************/

void TimA2_Cap_Init(void)
{
    // 1.复用输出
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(CAP_PORT_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    /* 定时器配置参数 分辨力 0.33us*/
    Timer_A_ContinuousModeConfig continuousModeConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_16, // SMCLK/16 = 3MHz
        TIMER_A_TAIE_INTERRUPT_DISABLE, // 开启定时器溢出中断
        TIMER_A_DO_CLEAR                // Clear Counter
    };
    // 3.将定时器初始化为连续计数模式
    MAP_Timer_A_configureContinuousMode(CAP_TIMA_SELECTION, &continuousModeConfig);

    // 4.配置捕捉模式结构体 */
    const Timer_A_CaptureModeConfig captureModeConfig_TA2 = {
        CAP_REGISTER_SELECTION,                      //在这里改引脚
        TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE, //上升下降沿捕获
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,           //CCIxA:外部引脚输入  （CCIxB:与内部ACLK连接(手册)
        TIMER_A_CAPTURE_SYNCHRONOUS,                 //同步捕获
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,     //开启CCRN捕获中断
        TIMER_A_OUTPUTMODE_OUTBITVALUE               //输出位值
    };
    // 5.初始化定时器的捕获模式
    MAP_Timer_A_initCapture(CAP_TIMA_SELECTION, &captureModeConfig_TA2);

    // 6.选择连续模式计数开始计数
    MAP_Timer_A_startCounter(CAP_TIMA_SELECTION, TIMER_A_CONTINUOUS_MODE);

    // 7.清除中断标志位
    //MAP_Timer_A_clearInterruptFlag(CAP_TIMA_SELECTION);                                   //清除定时器溢出中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION); //清除 CCR1 更新中断标志位
    MAP_Interrupt_enableInterrupt(INT_TA2_N);                                             //开启定时器A2端口中断
}
// 10.编写TIMA ISR ↓↓↓↓

// TIMA2_CAP_STA 捕获状态
uint8_t TIMA2_CAP_STA = 0;
uint16_t capTim[5];

uint16_t dt_1;
uint16_t dt_2;
uint16_t true_T = 60; // 真正的测量周期

// 过零比较器的误差
#define NOISE 120

void TA2_N_IRQHandler(void)
{
    static uint8_t i = 0;
    // 清除 CCR1 更新中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
    if (!TIMA2_CAP_STA) // 未捕获成功
    {
        if (i == 0)
        {
            MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);
        }
        else
        {
            capTim[i - 1] = MAP_Timer_A_getCaptureCompareCount(CAP_TIMA_SELECTION, CAP_REGISTER_SELECTION);
            if (i > 6)
            {
                // clear CCR1 and stop Timer
                MAP_Timer_A_stopTimer(CAP_TIMA_SELECTION);
                MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);
                i = 0;
                TIMA2_CAP_STA = 1;
                dt_1 = capTim[3] - capTim[1];
                dt_2 = capTim[4] - capTim[2];
                if (dt_1 <= (dt_2 + NOISE) && dt_1 >= (dt_2 - NOISE))
                {
                    true_T = dt_1;
                    //printf("一般\r\t");
                }
                else
                {
                    true_T = capTim[4] - capTim[0];
                    //printf("想多\r\t");
                }
                //printf("t4:%d\r\t", capTim[4]);
                return;
            }
        }
        ++i;
    }
}
