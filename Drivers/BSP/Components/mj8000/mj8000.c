#include "mj8000.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"

#define MJ8000_UART_RX_BUF_SIZE        (128)


static struct
{
    uint8_t buf[MJ8000_UART_RX_BUF_SIZE];       /* 帧接收缓冲 */
    uint16_t len;                               /* 帧接收长度 */
    bool finsh;                                 /* 帧接收完成标志 */
} mj_uart_rx_frame = {0};                        /* UART接收帧缓冲信息结构体 */



const char set_config_cmd[] = {
0x7E, 0x00, 0x08, 0xD0, 0x00, 0x00,
0xD6, 0x04, 0x00, 0x00, 0x00, 0x05, 0x32, 0x01, 0x2C, 0x04, 0x00, 0x3C, 0x05, 0xA2, 0x14, 0x64,
0x0A, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x01, 0x06, 0x64, 0x03, 0x03,
0x03, 0x03, 0x03, 0x01, 0x00, 0xFF, 0x21, 0x00, 0xFF, 0x01, 0x00, 0xFF, 0x01, 0x00, 0xFF, 0x21,
0x00, 0x04, 0x20, 0x00, 0x04, 0x20, 0x00, 0x04, 0x20, 0x00, 0x04, 0x20, 0x00, 0x04, 0x20, 0x00,
0x00, 0x00, 0x04, 0x20, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00,
0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x64, 0x64, 0x63, 0x63, 0x63, 0x6A, 0x62, 0x69, 0x61, 0x65, 0x44, 0x76, 0x48, 0x6D, 0x52,
0x52, 0x52, 0x51, 0x75, 0x72, 0x58, 0x68, 0x52, 0x66, 0x6E, 0x58, 0x58, 0x58, 0x78, 0x7A, 0x52,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x20, 0x00, 0x04,
0x20, 0x00, 0x04, 0x20, 0x00, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xAB, 0xCD
};

const char save_flash_cmd[] = {
0x7E, 0x00, 0x09, 0x01, 0x00, 0x00,
0x00,
0xDE, 0xC8
};


unsigned int crc_cal_by_bit(unsigned char* ptr, unsigned int len)
{
    unsigned int crc = 0;

    while (len-- != 0)
    {
        for(unsigned char i = 0x80; i != 0; i/=2)
        {
            crc *= 2;
            if((crc & 0x10000) != 0) //上一位crc*2, 若首位是1, 则除以0x11021
                crc ^= 0x11021;
            if((*ptr & i) != 0) //如果本位是1, 那么CRC= 上一位的CRC + 本位/CRC_CCITT
                crc ^= 0x1021;
        }
        ptr++;
    };

    return crc;
}

void mj8000_task_handle(void)
{
    if(mj_uart_rx_frame.finsh)
    {
        mj_uart_rx_frame.buf[mj_uart_rx_frame.len] = '\0';
        LOG_I("[MJ]recv len:%d,data:%s\r\n", mj_uart_rx_frame.len, mj_uart_rx_frame.buf);

        uart4_sync_output(mj_uart_rx_frame.buf, mj_uart_rx_frame.len);

        mj_uart_rx_frame.finsh = 0;
        uart4_recive_dma(mj_uart_rx_frame.buf, MJ8000_UART_RX_BUF_SIZE);
    }
    // uart4_sync_output(save_flash_cmd, sizeof(save_flash_cmd));
}


void mj8000_uart_rx_callback(UART_HandleTypeDef *huart)
{
    mj_uart_rx_frame.len = MJ8000_UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    mj_uart_rx_frame.finsh = 1;                                      /* 标记帧接收完成 */
}


void mj8000_init(void)
{
    LOG_I("%s", __FUNCTION__);

    uart4_init();

    mj_uart_rx_frame.finsh = 0;
    uart4_recive_dma(mj_uart_rx_frame.buf, MJ8000_UART_RX_BUF_SIZE);
}
