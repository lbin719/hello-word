#include "led.h"
#include "ulog.h"
#include "stm32f1xx_hal.h"
#include "board.h"
#include "cmsis_os.h"

#define LED_RED_ON()        	    (LED_RED_GPIO_PORT->BSRR = LED_RED_GPIO_PIN)
#define LED_RED_OFF()       		(LED_RED_GPIO_PORT->BSRR = (uint32_t)(LED_RED_GPIO_PIN  << 16u))

#define LED_GREEN_ON()        	    (LED_GREEN_GPIO_PORT->BSRR = (uint32_t)(LED_GREEN_GPIO_PIN  << 16u))
#define LED_GREEN_OFF()       		(LED_GREEN_GPIO_PORT->BSRR = LED_GREEN_GPIO_PIN)

static osTimerId led_timehandle = NULL;
static bool led_status = false;

static void led_ostimercallback(void const * argument)
{
    (void) argument;

    if(!led_status)
        return ;

    HAL_GPIO_TogglePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN);
}

static void led_control(bool on, uint32_t timer)
{
    led_status = on;
    if(led_status)
    {
        if(timer > 0)
        	osTimerStart(led_timehandle, timer);
        else
        	osTimerStop(led_timehandle);
        LED_RED_ON();
    }
    else
    {
        osTimerStop(led_timehandle);
        LED_RED_OFF();
    }
}

void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    LED_RED_GPIO_CLK_ENABLE();
    LED_GREEN_GPIO_CLK_ENABLE();

    LED_RED_OFF();
    LED_GREEN_OFF();

    gpio_init_struct.Pin = LED_RED_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(LED_RED_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LED_GREEN_GPIO_PIN;
    HAL_GPIO_Init(LED_GREEN_GPIO_PORT, &gpio_init_struct);

    osTimerDef(led_timer, led_ostimercallback);
    led_timehandle = osTimerCreate(osTimer(led_timer), osTimerPeriodic, NULL);
    assert_param(led_timehandle);

    led_control(true, 50);
    LOG_I("%s\r\n", __FUNCTION__);
}
