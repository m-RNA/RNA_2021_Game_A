#include "log.h"
//#include "usart.h"
// #include "stdarg.h"

//#ifdef __GNUC__
//#define PUTCHAR_PROTOTYPE int _io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__*/

//PUTCHAR_PROTOTYPE
//{
//    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
//    return ch;
//}

// void __log_print(char *fmt, ...)
// {
//     va_list arg;
//     va_start(arg, fmt);
//     char buf[1 + vsnprintf(NULL, 0, fmt, arg)];
//     vsnprintf(buf, sizeof(buf), fmt, arg);
//     va_end(arg);
//     printf("%s\n", buf);
// }
