#include "key.h"
#include "ulog.h"
#include "stm32f1xx_hal.h"
#include "board.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    KEY_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = KEY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(KEY_GPIO_PORT, &gpio_init_struct);

    LOG_I("%s\r\n", __FUNCTION__);
}

void key_task_handle(void)
{

}