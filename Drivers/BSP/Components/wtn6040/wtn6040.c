#include "wtn6040.h"
#include "board.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"
#include "delay.h"

#define WTN6040_DATA(x)     do{ x ? \
                                  HAL_GPIO_WritePin(WTN6040_DATA_GPIO_PORT, WTN6040_DATA_GPIO_PIN, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(WTN6040_DATA_GPIO_PORT, WTN6040_DATA_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)

#define WTN6040_PLAY(x)     (0x00 + x)// 语音索引
#define WTN6040_LEVEL(x)    (0xE0 + x)// 调节音量0~15

void wtn6040_task_handle(void)
{

}

void wtn6040_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    WTN6040_DATA_GPIO_CLK_ENABLE();
    WTN6040_BUSY_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = WTN6040_DATA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(WTN6040_DATA_GPIO_PORT, &gpio_init_struct);
    WTN6040_DATA(1);

    // gpio_init_struct.Pin = WTN6040_BUSY_GPIO_PIN;
    // gpio_init_struct.Mode = GPIO_MODE_INPUT;
    // HAL_GPIO_Init(WTN6040_BUSY_GPIO_PORT, &gpio_init_struct);

    LOG_I("%s\r\n", __FUNCTION__);
}

void wtn6040_write_data(uint8_t data)
{
    WTN6040_DATA(1);
    WTN6040_DATA(0);
    delay_ms(5);
    for(uint8_t i = 0; i < 8; i++)
	{
        WTN6040_DATA(1);
        if(data & 0x01)
        {
            delay_us(600);
            WTN6040_DATA(0);
            delay_us(200);
        }
        else
        {
            delay_us(200);
            WTN6040_DATA(0);
            delay_us(600);
        }
        data = data >> 1;
	}
    WTN6040_DATA(1);
}