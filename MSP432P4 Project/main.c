/*******************************************
// MSP432P401R
// 2021年 电赛A题
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/11/11
*******************************************/

#include "sysinit.h"
#include "usart.h"
#include "led.h"
#include "timA.h"
#include "delay.h"
#include "baseboard.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "adc.h"
#include "usart3.h"
#include "gameA.h"

// 是否调试 是则定义 DEBUG
#define DEBUG

float fft_outputbuf[ADC_SAMPLING_NUMBER * MM];           // FFT输出数组
static float fft_inputbuf[ADC_SAMPLING_NUMBER * 2 * MM]; // FFT输入数组

int main(void)
{
    uint16_t i;
    uint16_t j;
    uint8_t key_val; // 按键键值
    char strBuf[9];  // OLED_printf暂存

    float THDx;                  // THDx
    float gyh[4] = {0, 0, 0, 0}; //归一化幅值

    /***   三大初始化函数   ***/
    SysInit();       // 第3讲 时钟配置（48M）
    delay_init();    // 第4讲 滴答延时
    BaseBoardInit(); // 第2讲 GPIO (KEY LED BEEP OLED)

    BEEP = 0; // 打开蜂鸣器

    /**  显示 Ti和电赛 Logo  **/
    DrawBitmap(0, 0, TiLOGO, 128, 64);   // Ti logo
    UpdateScreen();                      // 更新屏幕
    SelectDownOLED();                    // 选用下屏
    ClearScreen();                       // 清屏
    DrawBitmap(0, 0, GameLOGO, 128, 64); // 电赛 logo
    UpdateScreen();                      // 更新屏幕

    uart_init(1382400);    // 第7讲 串口配置 （调试）
    usart3_init(9600);     // 第7讲 串口配置 （蓝牙）
    TimA0_Int_Init(60, 1); // 第8讲 定时器配置 （ADC触发时钟源 fs）
    TimA2_Cap_Init();      // 第8讲 定时器捕获 （过零比较器采频率）
    adc_dma_init(1024);    // 第12讲 DMA
    BEEP = 1;              // 关闭 蜂鸣器
    ADC_Config();          // 第11讲 ADC

    /* 初始化完毕 提示可以测量 */
    printf("Hello,MSP432!\r\n");
    DrawString(0, 0, "OK");
    UpdateScreen();

    MAP_Interrupt_enableMaster(); // 开启总中断
    while (1)
    {
        key_val = KEY_Scan(0); //扫描键值

        switch (key_val)
        {
        case KEY1_OnBoard_PRES: // KEY1 按下， 开始一键测量

            /*****************************   测量f、调整fs   ******************************/

            LED_B = 0;         // 关蓝灯
            LED_G = 1;         // 开绿灯
            TIMA2_CAP_STA = 0; // 捕获完成标志位清零

            MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);                            // 捕获定时器清零
            MAP_Timer_A_startCounter(CAP_TIMA_SELECTION, TIMER_A_CONTINUOUS_MODE); // 开始测量f

            delay_ms(200); // 等待f测量完成

            MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, true_T); // 调整fs

            WaitingAnimat(1); // 等待动画（等待定时器A稳定）(...)

            LED_G = 0; //关绿灯

            /********************************   开启DMA传输   ********************************/

            MAP_DMA_setChannelTransfer(DMA_CH7_ADC14 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, (void *)&ADC14->MEM[0], (void *)adc_inputbuf, 1024);
            MAP_DMA_enableChannel(7); // 使能7通道（ADC）

            MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); // 开始计数 触发ADC定时采样

            recv_done_flag = 0;     // 传输完成标志位清零
            while (!recv_done_flag) // 等待传输完成
                ;

            /**********************************   FFT 计算   **********************************/
            LED_RED = 1; // 开红灯

            /**  将数据转换为复数  **/
            for (j = 0; j < MM; ++j)
            {
                for (i = 0; i < ADC_SAMPLING_NUMBER; ++i)
                {
                    fft_inputbuf[2 * i] = adc_inputbuf[j][i]; // 实部为ADC
                    fft_inputbuf[2 * i + 1] = 0;              // 虚部为0
                }
            }

            arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);               // FFT计算
            arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, ADC_SAMPLING_NUMBER * MM); //把运算结果复数求模得幅值

            LED_RED = 0; // 关红灯

            /********************      找出基波谐波位置 计算归一化幅值      *******************/

            /* 找出基波位置 */
            f0 = FloatMax(fft_outputbuf);

            for (i = 0; i < 4; ++i)
            {
                /**   找出谐波位置   **/
                fx[i] = FloatMax_WithWindow(fft_outputbuf, f0 * (i + 2) - (FDBS / 2), f0 * (i + 2) + (FDBS / 2)); // 优化过的算法 更加准确

                /**  计算归一化幅值  **/
                gyh[i] = floor(fft_outputbuf[fx[i]] / fft_outputbuf[f0] * 100.0f) / 100.0f; // 向下取整 误差更小
            }

            /****************************   THD计算与显示到OLED   ****************************/

            THDx = THDx_calculate(); // 计算THDx
            UpdateGYH(gyh, THDx);    // 更新到屏幕

            /*********************   计算波形后显示到OLED与发送至手机   *********************/

            ShowWave_AndTran(gyh); //计算、转换并显示波形

            BEEP = 0;                               // 开始鸣叫
            BluetoothSendDate(gyh, THDx, waveTran); // 发送数据给手机
            BEEP = 1;                               // 停止鸣叫

// 定义DEBUG 则开启打印内部信息
#ifdef DEBUG
            printf("\r\nADC采样数据:\r\n");
            for (j = 0; j < MM; ++j)
            {
                for (i = 0; i < ADC_SAMPLING_NUMBER; ++i)
                {
                    printf("%d\r\n", adc_inputbuf[j][i]);
                }
            }

            printf("\r\nFFT后求幅值数据:\r\n");
            for (i = 0; i < ADC_SAMPLING_NUMBER; ++i)
            {
                printf("[%d]:%.3f\r\n", i, fft_outputbuf[i]);
            }

            printf("\r\n基波周期：%.2fus\r\n", true_T / 3.0f);
            printf("基波位置：%d\r\n", f0);
            for (i = 0; i < 4; ++i)
            {
                printf("%d谐位置：%d\r\n", i + 2, f0);
            }

            printf("\r\nTHD：%2.2f\r\n", THDx);

            printf("归一化幅值：\r\n");
            printf("基:1.00\r\n");
            for (i = 0; i < 4; ++i)
            {
                printf("%d:%1.2f\r\n", i + 2, gyh[i]);
            }
#endif
            break;

        case KEY2_PRES:   // 测试单片机是否卡死
            LED_RED ^= 1; // 翻转红灯电平
            break;

        case KEY3_PRES:   // 测试过零比较器是否正常工作
            LED_RED ^= 1; // 翻转红灯电平
            TIMA2_CAP_STA = 0;
            MAP_Timer_A_clearTimer(CAP_TIMA_SELECTION);
            MAP_Timer_A_startCounter(CAP_TIMA_SELECTION, TIMER_A_CONTINUOUS_MODE);
            snprintf(strBuf, 9, "T:%4dus", true_T / 3); // 测量周期
            DrawString(80, 0, strBuf);
            UpdateScreen();

            break;

        case KEY4_PRES:   // 测试蓝牙是否正确连接
            LED_RED ^= 1; // 翻转红灯电平
            MAP_UART_transmitData(HC_05_USART_BASE, 1920);
            break;
        }

        key_val = NOT_PRES; //清零键值
        delay_ms(10);       //延时10ms
    }
}
