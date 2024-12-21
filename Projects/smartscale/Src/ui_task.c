#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#include "lcd.h"
#include "fonts.h"
#include "text.h"
#include "ulog.h"
#include "ui_task.h"
#include "version.h"
#include "gbk.h"
#include "cmsis_os.h"
#include "lcd.h"


osThreadId UI_ThreadHandle;


static char disp_str[64] = {0};


int32_t ui_ossignal_notify(int32_t signals)
{
    if(!UI_ThreadHandle)
    {
        return -1;
    }

    return osSignalSet(UI_ThreadHandle, signals);
}

#include "lvgl.h"

static void UI_Thread(void const *argument)
{
  osEvent event = {0};

  /* 系统其他硬件初始化信息，这里忽略 */
  // btim_timx_int_init(10-1,7200-1); /* 根据自己的开发板 MCU 定义定时器周期为 1ms */
  lv_init(); /* lvgl 系统初始化 */
  lv_port_disp_init(); /* lvgl 显示接口初始化,放在 lv_init()的后面 */
  lv_port_indev_init(); /* lvgl 输入接口初始化,放在 lv_init()的后面 */
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label,"Hello Alientek!!!");
  lv_obj_center(label);
  while(1)
  {
    lv_timer_handler(); /* LVGL 管理函数相当于 RTOS 触发任务调度函数 */
    osDelay(5);
  }


  while(1) 
  {
    event = osSignalWait(UI_TASK_NOTIFY, osWaitForever);
    if(event.status == osEventSignal)
    {
      if(event.value.signals & UI_NOTIFY_LOCK_BIT)
      {
        //draw lines
        lcd_draw_hline(BLACK, 5, 60, 480-5*2);
        lcd_draw_hline(BLACK, 5, 265, 480-5*2);
        lcd_draw_vline(BLACK, 240, 265 + 5, 55-5*2);
      }
    }
  }
}

void ui_init(void)
{
  osThreadDef(UIThread, UI_Thread, osPriorityAboveNormal, 0, 512);
  UI_ThreadHandle = osThreadCreate(osThread(UIThread), NULL);
}
