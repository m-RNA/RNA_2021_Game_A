#include "log.h"
#include "stdarg.h"

void __log_print(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    char buf[1 + vsnprintf(NULL, 0, fmt, arg)];
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);
    printf("%s\n", buf);
}
