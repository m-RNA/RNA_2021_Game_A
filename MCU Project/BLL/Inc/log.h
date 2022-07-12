#ifndef __LOG_H__
#define __LOG_H__
#include "config.h"
#include "stdio.h"

#ifdef DEBUG
#define log_debug(fmt, ...) \
    printf("[Log Debug] "); \
    printf(fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...) ((void)0)
#endif

#ifdef DEBUG_PRINT_INTERNAL_DATA
#define log_indata(fmt, ...) \
    printf(fmt, ##__VA_ARGS__)
#else
#define log_indata(fmt, ...) ((void)0)
#endif

#define log_assert(fmt, ...)                                                                       \
    printf("[Log Assert] File: %s, Function: %s, Line: %d\r\n", __FILE__, __FUNCTION__, __LINE__); \
    printf(fmt, ##__VA_ARGS__);                                                                    \
    while (1)

// void __log_print(char *fmt, ...);

#endif
