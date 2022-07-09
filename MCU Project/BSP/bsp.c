#include "bsp.h"
#include "log.h"
#include "stdlib.h"

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

void BSP_ADC_DMA_Start(u16 *Data, u16 Num)
{
    return;
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


