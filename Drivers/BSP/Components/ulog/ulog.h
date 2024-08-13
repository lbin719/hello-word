#ifndef _ULOG_H
#define _ULOG_H

#include "stm32f1xx.h"
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef LOG_DEBUG_ENABLE
#define LOG_I(...)               ulog_sync_output(true,  __VA_ARGS__)
#define LOG_I_NOTICK(...)        ulog_sync_output(false,  __VA_ARGS__)
#else
#define LOG_I(...)
#define LOG_I_NOTICK(...)
#endif

#ifdef LOG_DEBUG_ENABLE
void ulog_init(void);
int ulog_sync_output(bool output_tick, const char* format, ...);
void usmart_scan(void);
#endif

#ifdef __cplusplus
 }
#endif

#endif /* _UART_LOG_H */