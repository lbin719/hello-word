
#include "uart.h"
#include "ulog.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef  LOG_DEBUG_ENABLE

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


#define USART_REC_LEN               128         /* 定义最大接收字节数 */

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
*/
volatile uint16_t g_usart_rx_sta = 0;

void usmart_recive_callback(uint8_t *rx_data, uint16_t len)
{
    while(len--)
    {
        if ((g_usart_rx_sta & 0x8000) == 0)             /* 接收未完成 */
        {
            if (g_usart_rx_sta & 0x4000)                /* 接收到了0x0d（即回车键） */
            {
                if (*rx_data != 0x0a)                   /* 接收到的不是0x0a（即不是换行键） */
                {
                    g_usart_rx_sta = 0;                 /* 接收错误,重新开始 */
                }
                else                                    /* 接收到的是0x0a（即换行键） */
                {
                    g_usart_rx_sta |= 0x8000;           /* 接收完成了 */
                }
            }
            else                                        /* 还没收到0X0d（即回车键） */
            {
                if (*rx_data == 0x0d)
                    g_usart_rx_sta |= 0x4000;
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = *rx_data;
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;             /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        rx_data++;
    }
}

/**
 * @brief       获取输入数据流(字符串)
 *   @note      USMART通过解析该函数返回的字符串以获取函数名及参数等信息
 * @param       无
 * @retval
 *   @arg       0,  没有接收到数据
 *   @arg       其他,数据流首地址(不能是0)
 */
char *usmart_get_input_string(void)
{
    uint8_t len;
    char *pbuf = 0;

    if (g_usart_rx_sta & 0x8000)        /* 串口接收完成？ */
    {
        len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
        g_usart_rx_buf[len] = '\0';     /* 在末尾加入结束符. */
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;             /* 开启下一次接收 */
    }

    return pbuf;
}

void usmart_scan(void)
{
    char *pbuf = 0;

    pbuf = usmart_get_input_string();   /* 获取数据数据流 */
    if (pbuf == 0) return ; /* 数据流空, 直接返回 */

    uart1_sync_output((uint8_t *)pbuf, strlen(pbuf));
}

#endif