#ifndef __LOG_H__
#define __LOG_H__
#include "main.h"
#include "config.h"

#ifdef DEBUG 
#define log_debug(fmt, ...) __log_print(fmt, ##__VA_ARGS__)
#else
#define log_debug(fmt, ...) ((void)0)
#endif
void __log_print(char *fmt, ...);

#endif
