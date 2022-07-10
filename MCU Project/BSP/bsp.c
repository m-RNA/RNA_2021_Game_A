#include "bsp.h"
#include "log.h"
#include "stdlib.h"
#include "math.h"

void BSP_Sample_ADC_with_DMA_Init(void)
{
    log_debug("config BSP_Sample_ADC_with_DMA_Init...\r\n");
}

void BSP_Sample_Timer_Init(void)
{
    log_debug("config BSP_Sample_Timer_Init...\r\n");
}

void BSP_Uart_PC(void)
{
    log_debug("config BSP_Uart_PC...\r\n");
}

void BSP_Uart_Bluetooth(void)
{
    log_debug("config BSP_Uart_Bluetooth...\r\n");
}

void BSP_LED_Init(void)
{
    log_debug("config BSP_LED_Init...\r\n");
}

void BSP_BEEP_Init(void)
{
    log_debug("config BSP_BEEP_Init...\r\n");
}

void BSP_KEY_Init(void)
{
    log_debug("config BSP_KEY_Init...\r\n");
}

void BSP_Init(void)
{
    BSP_Uart_PC();
    BSP_Uart_Bluetooth();
    BSP_LED_Init();
    BSP_BEEP_Init();
    BSP_KEY_Init();
}

void BSP_Set_Fs_CCR(u32 CCR)
{
    return;
}
u32 BSP_Get_Signal_CCR(void)
{
    return (rand() & 0xFF);
}


#define Y_RANGE_OF_WAVE 54
#define Y_UP_MOVE 64

void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
    float NormAm[4] = {0.5f, 0, 0.2f,0};
    
    for (u16 i = 0; i < Num; ++i)
    {
        *Data = Y_UP_MOVE;
        *Data += Y_RANGE_OF_WAVE * sin(PI * i / 64.0f);
        // OriginalWaveDate[i] = Y_RANGE_OF_WAVE * arm_sin_f32(PI * i / 64.0f);
        for (u16 j = 0; j < 4; ++j)
        {
            *Data += Y_RANGE_OF_WAVE * sin(PI * i * (j + 1) / 64.0f) * NormAm[j];
            // OriginalWaveDate[i] += Y_RANGE_OF_WAVE * arm_sin_f32(PI * i * (j + 1) / 64.0f) * NormAm[j];
        }
        ++Data;
    }
}

void NVIC_Init(void)
{
    log_debug("config NVIC...\r\n");
}

void Clock_Init(void)
{
    log_debug("config Clock_Init...\r\n");
}

void Delay_Init(void)
{
    log_debug("config Delay_Init...\r\n");
}



//void HAL_Init(void)
//{
//    NVIC_Init();
//    Clock_Init();
//    Delay_Init();
//}


