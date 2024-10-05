#ifndef _MJ8000_H
#define _MJ8000_H

#include "stdint.h"
#include "stdbool.h"

#define MJ8000_UART_RX_BUF_SIZE (64)

typedef struct
{
    uint8_t buf[MJ8000_UART_RX_BUF_SIZE];       /* 帧接收缓冲 */
    uint16_t len;                               /* 帧接收长度 */
    bool finsh;                                 /* 帧接收完成标志 */
} mj_uart_rx_t;                      /* UART接收帧缓冲信息结构体 */

extern mj_uart_rx_t mj_uart_rx_frame;

void mj8000_setconfig(void);
void mj8000_init(void);

#endif /* _MJ8000_H */

