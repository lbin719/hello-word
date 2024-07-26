
#include "uart.h"
// #include "SEGGER_RTT.h"
// #include "flash_if.h"
#include "ulog.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
// #include "stm32g0xx_hal.h"
// #include "cmsis_os.h"


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


#define MAX_HEX_STR         4
#define MAX_HEX_STR_LENGTH  128
char hexStr[MAX_HEX_STR][MAX_HEX_STR_LENGTH];
uint8_t hexStrIdx = 0;

char* hex2Str(unsigned char * data, size_t dataLen)
{
  unsigned char *pin = data;
  const char *hex = "0123456789ABCDEF";
  char *pout = hexStr[hexStrIdx];
  uint8_t i = 0;
  uint8_t idx = hexStrIdx;

  if( dataLen > (MAX_HEX_STR_LENGTH/2) )
  {
    dataLen = (MAX_HEX_STR_LENGTH/2);
  }

  if(dataLen == 0)
  {
    pout[0] = 0;
  }
  else
  {
    for(; i < dataLen - 1; ++i)
    {
        *pout++ = hex[(*pin>>4)&0xF];
        *pout++ = hex[(*pin++)&0xF];
    }
    *pout++ = hex[(*pin>>4)&0xF];
    *pout++ = hex[(*pin)&0xF];
    *pout = 0;
  }

  hexStrIdx++;
  hexStrIdx %= MAX_HEX_STR;

  return hexStr[idx];
}
