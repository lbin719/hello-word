#include "ui_task.h"
#include "stdio.h"
#include "main.h"
#include "hx711.h"

uint32_t weitht_lasttime = 0;
uint32_t last_weight = 0;

void weight_task_handle(void)
{
    if(HAL_GetTick() - weitht_lasttime < 200)
        return ;
    weitht_lasttime = HAL_GetTick();

	uint32_t current_weight = hx711_get_weight();
	if(current_weight != last_weight)
	{
		int len = snprintf(ui_draw.weight_str, STRING_NUM_LEN, "%ld", current_weight);
		if(len > STRING_NUM_LEN)
			Error_Handler();
		ui_draw.weight_str[len] = '\0';
		// LOG_I("ui_draw.weight_str:%s\r\n", ui_draw.weight_str);
		set_draw_update_bit(DRAW_UPDATE_WEIGHT_BIT);

		last_weight = current_weight;
	}
}