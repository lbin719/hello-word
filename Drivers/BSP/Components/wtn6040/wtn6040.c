#include "wtn6040.h"
#include "board.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"
#include "delay.h"
#include "cmsis_os.h"

#define WTN6040_DATA(x)     do{ x ? \
                                  HAL_GPIO_WritePin(WTN6040_DATA_GPIO_PORT, WTN6040_DATA_GPIO_PIN, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(WTN6040_DATA_GPIO_PORT, WTN6040_DATA_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)


#define WTN6040_PLAY(x)     (0x00 + x)// 语音索引
#define WTN6040_LEVEL(x)    (0xE0 + x)// 调节音量0~15


void wtn6040_write_data(uint8_t data)
{
    WTN6040_DATA(1);
    WTN6040_DATA(0);
    osDelay(5);
    portENTER_CRITICAL();
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
    portEXIT_CRITICAL();
}


void wtn6040_play(uint8_t index)
{
    LOG_I("[WTN]play:%d\r\n", index);
    
    if(index > WTN6040_MAX_PLAY)
        index = WTN6040_MAX_PLAY;

    wtn6040_write_data(WTN6040_PLAY(index));
}

void wtn6040_set_voice(uint8_t level)
{
    LOG_I("[WTN]set voice:%d\r\n", level);

    if(level > WTN6040_MAX_VOICE)
        level = WTN6040_MAX_VOICE;

    wtn6040_write_data(WTN6040_LEVEL(level));
}

void wtn6040_set_voice_store(uint8_t level)
{
    LOG_I("[WTN]set voice store :%d\r\n", level);

    if(level > WTN6040_MAX_VOICE)
        level = WTN6040_MAX_VOICE;

    wtn6040_write_data(WTN6040_LEVEL(level));
    sysinfo_store_voice(level);
}

void wtn6040_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    WTN6040_DATA_GPIO_CLK_ENABLE();
    WTN6040_BUSY_GPIO_CLK_ENABLE();

    WTN6040_DATA(1);
    gpio_init_struct.Pin = WTN6040_DATA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(WTN6040_DATA_GPIO_PORT, &gpio_init_struct);

    // gpio_init_struct.Pin = WTN6040_BUSY_GPIO_PIN;
    // gpio_init_struct.Mode = GPIO_MODE_INPUT;
    // HAL_GPIO_Init(WTN6040_BUSY_GPIO_PORT, &gpio_init_struct);

    uint8_t voice = sysinfo_get_voice();
    wtn6040_set_voice(voice);   

    LOG_I("%s\r\n", __FUNCTION__);
}

