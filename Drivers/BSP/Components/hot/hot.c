#include "hot.h"
#include "board.h"
#include "ulog.h"
#include  "system_info.h"
#include "cmsis_os.h"

#define HOT_CTRL_ON()        	    (HOT_CTRL_GPIO_PORT->BSRR = HOT_CTRL_GPIO_PIN)
#define HOT_CTRL_OFF()       		(HOT_CTRL_GPIO_PORT->BSRR = (uint32_t)(HOT_CTRL_GPIO_PIN  << 16u))

static osTimerId hot_timehandle = NULL;

uint8_t hot_mode = 0;
uint8_t hot_time = 0;

static void hot_ostimercallback(void const * argument)
{
    (void) argument;
    HOT_CTRL_OFF();
}

static void hot_ctrl(uint8_t mode, uint8_t time)
{
    LOG_I("[HOT]mode:%d time:%d\r\n", mode, time);

    osTimerStop(hot_timehandle);
    hot_mode = mode;
    hot_time = time;
    if(mode == HOT_ON)
    {
        HOT_CTRL_ON();
        osTimerStart(hot_timehandle, (uint32_t)time*60*1000);
    }
    else
    {
        HOT_CTRL_OFF();
    }
}

void hot_ctrl_store(uint8_t mode, uint8_t time)
{
    hot_ctrl(mode, time);
    sysinfo_store_hot(mode, time);
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

    osTimerDef(hot_timer, hot_ostimercallback);
    hot_timehandle = osTimerCreate(osTimer(hot_timer), osTimerPeriodic, NULL);
    assert_param(hot_timehandle);

    sysinfo_get_hot(&hot_mode, &hot_time);
    hot_ctrl(hot_mode, hot_time);

    LOG_I("%s mode:%d, time:%d\r\n", __FUNCTION__, hot_mode, hot_time);
}
