#include "hot.h"
#include "board.h"
#include "ulog.h"


#define HOT_CTRL_ON()        	    (HOT_CTRL_GPIO_PORT->BSRR = HOT_CTRL_GPIO_PIN)
#define HOT_CTRL_OFF()       		(HOT_CTRL_GPIO_PORT->BSRR = (uint32_t)(HOT_CTRL_GPIO_PIN  << 16u))


void hot_ctrl(uint8_t ctrl)
{
    if(ctrl == HOT_ON)
    {
        HOT_CTRL_ON();
    }
    else
    {
        HOT_CTRL_OFF();
    }
}

void hot_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    HOT_CTRL_GPIO_CLK_ENABLE();

    HOT_CTRL_OFF();
    gpio_init_struct.Pin = HOT_CTRL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(HOT_CTRL_GPIO_PORT, &gpio_init_struct);



    LOG_I("%s\r\n", __FUNCTION__);
}