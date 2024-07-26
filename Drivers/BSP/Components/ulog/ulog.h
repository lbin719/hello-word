#ifndef _ULOG_H
#define _ULOG_H

#include "stm32f1xx.h"
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define LOG_I(...)               ulog_sync_output(true,  __VA_ARGS__)
#define LOG_I_NOTICK(...)        ulog_sync_output(false,  __VA_ARGS__)

void ulog_init(void);
int ulog_sync_output(bool output_tick, const char* format, ...);


char* hex2Str(unsigned char * data, size_t dataLen);

#ifdef __cplusplus
 }
#endif

#endif /* _UART_LOG_H */