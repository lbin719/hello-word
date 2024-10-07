
#include "uart.h"
#include "ulog.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "cmsis_os.h"

#ifdef  LOG_DEBUG_ENABLE

#define ULOGGER_BUFFER_SIZE (256)

static char ulog_buf[ULOGGER_BUFFER_SIZE];
static bool ulog_is_init = false;
static osMutexId ulog_osMutexHandle = NULL;

void ulog_init(void)
{
    uart1_init();

    osMutexDef(ulog_osMutex);
    ulog_osMutexHandle = osMutexCreate(osMutex(ulog_osMutex));
    assert_param(ulog_osMutexHandle);

    ulog_is_init = true;
}

int ulog_sync_output(bool output_tick, const char* format, ...)
{
    int cnt = 0;

    if(!ulog_is_init)
    {
        return 0;
    }

    if (__get_IPSR() == 0)
    {
        osMutexWait(ulog_osMutexHandle, osWaitForever);
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

    if (__get_IPSR() == 0)
    {
        osMutexRelease(ulog_osMutexHandle);
    }

    return cnt;
}

#endif