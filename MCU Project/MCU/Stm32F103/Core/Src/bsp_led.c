#include "bsp_led.h"

void LED_Init(void)
{
    LED_RED_Off();
    LED_R_Off();
    LED_G_Off();
    LED_B_Off();
}

void LED_RED_Off(void) {}
void LED_R_Off(void) {}
void LED_G_Off(void) {}
void LED_B_Off(void) {}

void LED_RED_On(void) {}
void LED_R_On(void) {}
void LED_G_On(void) {}
void LED_B_On(void) {}

void LED_RED_Tog(void) {}
void LED_R_Tog(void) {}
void LED_G_Tog(void) {}
void LED_B_Tog(void) {}

//白色 White
void LED_W_On(void)
{
    LED_R_On();
    LED_G_On();
    LED_B_On();
}
//白色 White
void LED_W_Off(void)
{
    LED_R_Off();
    LED_G_Off();
    LED_B_Off();
}
//白色 White
void LED_W_Tog(void)
{
    LED_R_Tog();
    LED_G_Tog();
    LED_B_Tog();
}
//黄色 Yellow
void LED_Y_On(void)
{
    LED_R_On();
    LED_G_On();
    LED_B_Off();
}
//品红 Pinkish red
void LED_P_On(void)
{
    LED_R_On();
    LED_G_Off();
    LED_B_On();
}
//青色 Cyan
void LED_C_On(void)
{
    LED_R_Off();
    LED_G_On();
    LED_B_On();
}
