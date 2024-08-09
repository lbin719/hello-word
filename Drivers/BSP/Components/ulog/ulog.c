
#include "uart.h"
#include "ulog.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef  LOG_DEBUG_ENABLE

#define ULOGGER_BUFFER_SIZE (256)

static char ulog_buf[ULOGGER_BUFFER_SIZE];
static bool ulog_is_init = false;

void ulog_init(void)
{
    uart1_init();

    ulog_is_init = true;
}

int ulog_sync_output(bool output_tick, const char* format, ...)
{
    int cnt = 0;

    if(!ulog_is_init)
    {
        return 0;
    }

    if(output_tick)
    {
        cnt = snprintf(ulog_buf, sizeof(ulog_buf), "[%ld]", HAL_GetTick());
    }

    va_list argptr;
    va_start(argptr, format);
    cnt += vsnprintf(ulog_buf + cnt, sizeof(ulog_buf) - cnt, format, argptr);
    va_end(argptr);

    uart1_sync_output((uint8_t *)ulog_buf, cnt);

    return cnt;
}

#endif