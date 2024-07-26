#ifndef _UART_H
#define _UART_H

#include "stm32f1xx.h"
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif


void uart1_init(void);
void uart1_sync_output(const uint8_t *pData, uint16_t Size);

void uart2_init(void);

void uart3_init(void);
void uart3_sync_output(const uint8_t *pData, uint16_t Size);


#ifdef __cplusplus
 }
#endif

#endif /* _UART_LOG_H */