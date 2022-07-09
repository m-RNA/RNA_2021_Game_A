#ifndef __MAIN_H__
#define __MAIN_H__
#include "stdint.h"

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;
typedef volatile uint32_t vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t vu8;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef const uint32_t uc32; /*!< Read Only */
typedef const uint16_t uc16; /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

// #define KEY1_Pin GPIO_PIN_0
// #define KEY1_GPIO_Port GPIOB
// #define KEY2_Pin GPIO_PIN_1
// #define KEY2_GPIO_Port GPIOB
// #define LED_LOCK_Pin GPIO_PIN_2
// #define LED_LOCK_GPIO_Port GPIOD
// #define BEEP_Pin GPIO_PIN_3
// #define BEEP_GPIO_Port GPIOB

#define DEBUG

#endif
