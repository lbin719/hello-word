#include "ec800e.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"
#include <stdarg.h>
#include <stdio.h>
#include "board.h"
#include "ringbuffer.h"
#include "wl_task.h"

#define EC_RST_SET()        	    (EC_RST_GPIO_PORT->BSRR = (uint32_t)(EC_RST_GPIO_PIN  << 16u))
#define EC_RST_RESET()       		(EC_RST_GPIO_PORT->BSRR = EC_RST_GPIO_PIN)

#define EC_PWR_SET()        	    (EC_PWR_GPIO_PORT->BSRR = (uint32_t)(EC_PWR_GPIO_PIN  << 16u))
#define EC_PWR_RESET()       		(EC_PWR_GPIO_PORT->BSRR = EC_PWR_GPIO_PIN)


uart_rx_frame_t g_uart_rx_frame = {0};                        /* UART接收帧缓冲信息结构体 */
static uint8_t g_uart_tx_buf[EC800E_UART_TX_BUF_SIZE]; /* UART发送缓冲 */

#define ECUART_RX_BUF_SIZE   (256)
uint8_t ecuart_rx_buf[ECUART_RX_BUF_SIZE];

#define ECRX_BUF_SIZE        (512)
ring_buf_t ecrx_rb_handle = {0};
uint8_t ecrx_rbuf[ECRX_BUF_SIZE];

void ec800e_uart_output(char *buf, uint16_t len)
{
    uart2_sync_output(buf, len);
    LOG_I("[EC] T:%s", buf);
}

void ec800e_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);
    uart2_sync_output(g_uart_tx_buf, len);
    LOG_I("[EC] T:%s", g_uart_tx_buf);
}




void ec800e_uart_rx_callback(UART_HandleTypeDef *huart)
{
    ring_buf_put(&ecrx_rb_handle, ecuart_rx_buf, (ECUART_RX_BUF_SIZE -  __HAL_DMA_GET_COUNTER(huart->hdmarx)));
    wl_ossignal_notify(WL_NOTIFY_RECEIVE_BIT);

    uart2_recive_dma(ecuart_rx_buf, ECUART_RX_BUF_SIZE);   
}

uint32_t ec800e_get_rx_buf(uint8_t *buf, uint32_t len)
{
    if(ring_buf_len(&ecrx_rb_handle) == 0)
        return 0;

    __disable_irq();
    uint32_t len_act = ring_buf_get(&ecrx_rb_handle, buf, len);
    __enable_irq();

    // if(len_act > 0)
    {
        buf[len_act] = '\0';
        LOG_I("[EC]Recv len:%d :%s", len_act, buf);
    }

    return len_act;
}

void ec800e_clear_rx_buf(void)
{
    __disable_irq();
    ring_buf_clr(&ecrx_rb_handle);
    __enable_irq();
}

// void ec800e_start_recv(void)
// {
//     g_uart_rx_frame.finsh = 0;
//     uart2_recive_dma(g_uart_rx_frame.buf, EC800E_UART_RX_BUF_SIZE);
// }

// bool ec800e_wait_recv_data(void)
// {
//     if(g_uart_rx_frame.finsh)
//     {
//         g_uart_rx_frame.buf[g_uart_rx_frame.len] = '\0';
// #if 0
//         LOG_I("[EC]rx len:%d, hex:", g_uart_rx_frame.len);
//         for(uint32_t i = 0; i < g_uart_rx_frame.len; i++)
//         {
//             LOG_I_NOTICK(" %02X ", g_uart_rx_frame.buf[i]);
//         }
//         LOG_I_NOTICK("\r\n");
// #endif
//         LOG_I("[EC]rx: %s\r\n", g_uart_rx_frame.buf);

//         g_uart_rx_frame.finsh = 0;
//         return true;
//     }
//     return false;
// }

void ec800e_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    EC_PWR_GPIO_CLK_ENABLE();
    EC_RST_GPIO_CLK_ENABLE();
    gpio_init_struct.Pin = EC_PWR_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(EC_PWR_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = EC_RST_GPIO_PIN;
    HAL_GPIO_Init(EC_RST_GPIO_PORT, &gpio_init_struct);

    //POWER ON
    EC_RST_RESET();
    EC_PWR_SET();
    osDelay(30);
    EC_PWR_RESET();
    osDelay(300);
    EC_RST_SET();

    uart2_init();


    ring_buf_create(&ecrx_rb_handle, ecrx_rbuf, sizeof(ecrx_rbuf));
    uart2_recive_dma(ecuart_rx_buf, ECUART_RX_BUF_SIZE);
    // g_uart_rx_frame.finsh = 0;
    // uart2_recive_dma(g_uart_rx_frame.buf, EC800E_UART_RX_BUF_SIZE);

    LOG_I("%s\r\n", __FUNCTION__);
}
