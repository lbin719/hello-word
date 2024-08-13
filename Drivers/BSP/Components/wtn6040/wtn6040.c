#include "wtn6040.h"
#include "board.h"
#include "ulog.h"
#include "uart.h"
#include "stm32f1xx_hal.h"


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
    // HAL_GPIO_WritePin(WTN6040_DATA_GPIO_PORT, WTN6040_DATA_GPIO_PIN, GPIO_PIN_SET);

    gpio_init_struct.Pin = WTN6040_BUSY_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(WTN6040_BUSY_GPIO_PORT, &gpio_init_struct);


    LOG_I("%s", __FUNCTION__);
}
