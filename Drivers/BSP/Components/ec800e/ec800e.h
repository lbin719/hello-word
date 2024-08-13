#ifndef _EC800E_H
#define _EC800E_H

// #define UART4_RXDMA_BUFFER_SIZE (255)

void ec800e_init(void);
void ec800e_task_handle(void);

/* 操作函数 */
// void atk_idm750c_uart_printf(char *fmt, ...);           /* ATK-IDM750C UART printf */
// void atk_idm750c_uart_rx_restart(void);                 /* ATK-IDM750C UART重新开始接收数据 */
// uint8_t *atk_idm750c_uart_rx_get_frame(void);           /* 获取ATK-IDM750C UART接收到的一帧数据 */
// uint16_t atk_idm750c_uart_rx_get_frame_len(void);       /* 获取ATK-IDM750C UART接收到的一帧数据的长度 */
// void atk_idm750c_uart_init(uint32_t baudrate);          /* ATK-IDM750C UART初始化 */


#endif /* _EC800E_H */

