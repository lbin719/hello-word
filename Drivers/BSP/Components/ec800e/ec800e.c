#include "ec800e.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"
#include <stdarg.h>
#include <stdio.h>
#include "board.h"

#define EC800E_UART_RX_BUF_SIZE        (512)
#define EC800E_UART_TX_BUF_SIZE        (512)

static struct
{
    uint8_t buf[EC800E_UART_RX_BUF_SIZE];       /* 帧接收缓冲 */
    uint16_t len;                               /* 帧接收长度 */
    bool finsh;                                 /* 帧接收完成标志 */
} g_uart_rx_frame = {0};                        /* UART接收帧缓冲信息结构体 */

static uint8_t g_uart_tx_buf[EC800E_UART_TX_BUF_SIZE]; /* UART发送缓冲 */

void ec800e_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);
    uart2_sync_output(g_uart_tx_buf, len);
}

void ec800e_task_handle(void)
{
    if(g_uart_rx_frame.finsh)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.len] = '\0';
        LOG_I("[EC]recv: %s\r\n", g_uart_tx_buf);

        // ec800e_uart_printf("%s\r\n", g_uart_rx_frame.buf);

        g_uart_rx_frame.finsh = 0;
        uart2_recive_dma(g_uart_rx_frame.buf, EC800E_UART_RX_BUF_SIZE);

    }
    ec800e_uart_printf("AT\r\n");
}


void ec800e_uart_rx_callback(UART_HandleTypeDef *huart)
{
    // uint8_t tmp;
    // if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET)       /* UART接收中断 */
    // {
    //     HAL_UART_Receive(&g_uart_handle, &tmp, 1, HAL_MAX_DELAY);           /* UART接收数据 */

    //     if (g_uart_rx_frame.len < (EC800E_UART_RX_BUF_SIZE - 1))        /* 判断UART接收缓冲是否溢出
    //                                                                          * 留出一位给结束符'\0'
    //                                                                          */
    //     {
    //         g_uart_rx_frame.buf[g_uart_rx_frame.len] = tmp;             /* 将接收到的数据写入缓冲 */
    //         g_uart_rx_frame.len++;                                      /* 更新接收到的数据长度 */
    //     }
    //     else                                                                /* UART接收缓冲溢出 */
    //     {
    //         g_uart_rx_frame.len = 0;                                    /* 覆盖之前收到的数据 */
    //         g_uart_rx_frame.buf[g_uart_rx_frame.len] = tmp;             /* 将接收到的数据写入缓冲 */
    //         g_uart_rx_frame.len++;                                      /* 更新接收到的数据长度 */
    //     }
    // }

    g_uart_rx_frame.len = EC800E_UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    g_uart_rx_frame.finsh = 1;                                      /* 标记帧接收完成 */
}

void ec800e_init(void)
{
    LOG_I("%s\r\n", __FUNCTION__);

    GPIO_InitTypeDef gpio_init_struct = {0};

    EC_PWR_GPIO_CLK_ENABLE();
    gpio_init_struct.Pin = EC_PWR_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(EC_PWR_GPIO_PORT, &gpio_init_struct);
    HAL_GPIO_WritePin(EC_PWR_GPIO_PORT, EC_PWR_GPIO_PIN, GPIO_PIN_RESET);//

    uart2_init();

    g_uart_rx_frame.finsh = 0;
    uart2_recive_dma(g_uart_rx_frame.buf, EC800E_UART_RX_BUF_SIZE);
}
