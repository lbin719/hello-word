#ifndef _EC800E_H
#define _EC800E_H

#include <stdint.h>
#include <stdbool.h>

#define EC800E_UART_RX_BUF_SIZE        (256)
#define EC800E_UART_TX_BUF_SIZE        (256)

typedef struct
{
    uint8_t buf[EC800E_UART_RX_BUF_SIZE];       /* 帧接收缓冲 */
    uint16_t len;                               /* 帧接收长度 */
    bool finsh;                                 /* 帧接收完成标志 */
} uart_rx_frame_t; 

extern uart_rx_frame_t g_uart_rx_frame;

void ec800e_init(void);
void ec800e_uart_printf(char *fmt, ...);
void ec800e_start_recv(void);
bool ec800e_wait_recv_data(void);

/* 操作函数 */
// void atk_idm750c_uart_printf(char *fmt, ...);           /* ATK-IDM750C UART printf */
// void atk_idm750c_uart_rx_restart(void);                 /* ATK-IDM750C UART重新开始接收数据 */
// uint8_t *atk_idm750c_uart_rx_get_frame(void);           /* 获取ATK-IDM750C UART接收到的一帧数据 */
// uint16_t atk_idm750c_uart_rx_get_frame_len(void);       /* 获取ATK-IDM750C UART接收到的一帧数据的长度 */
// void atk_idm750c_uart_init(uint32_t baudrate);          /* ATK-IDM750C UART初始化 */


#endif /* _EC800E_H */

