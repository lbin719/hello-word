#include "led.h"
#include "ulog.h"
#include "stm32f1xx_hal.h"
#include "board.h"

#define LED_RED_ON()        	    (LED_RED_GPIO_PORT->BSRR = LED_RED_GPIO_PIN)
#define LED_RED_OFF()       		(LED_RED_GPIO_PORT->BSRR = (uint32_t)(LED_RED_GPIO_PIN  << 16u))

#define LED_GREEN_ON()        	    (LED_GREEN_GPIO_PORT->BSRR = (uint32_t)(LED_GREEN_GPIO_PIN  << 16u))
#define LED_GREEN_OFF()       		(LED_GREEN_GPIO_PORT->BSRR = LED_GREEN_GPIO_PIN)

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

    LOG_I("%s\r\n", __FUNCTION__);
}

void led_task_handle(void)
{
    static bool led_test;

    if(led_test)
    {
        LED_RED_OFF();
        LED_GREEN_OFF();
    }
    else
    {
        LED_RED_ON();
        LED_GREEN_ON();
    }

    led_test = !led_test;
}