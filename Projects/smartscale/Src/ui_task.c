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


int32_t ui_ossignal_notify(int32_t signals)
{
    if(!UI_ThreadHandle)
    {
        return -1;
    }

    return osSignalSet(UI_ThreadHandle, signals);
}

#include "lvgl.h"

/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static lv_obj_t *label_left;            /* 左侧百分比标签 */
static lv_obj_t *label_right;           /* 右侧百分比标签 */
static lv_obj_t *arc_left;              /* 左侧圆弧 */
static lv_obj_t *arc_right;             /* 右侧圆弧 */

static uint8_t arc_width;               /* 圆弧宽度 */

void lv_example_arc1(void)
{
    /* 根据活动屏幕宽度选择圆弧宽度 */
    if (scr_act_width() <= 480)
    {
        arc_width = 10;
    }
    else
    {
        arc_width = 20;
    }

    /* 左侧圆弧 */
    arc_left = lv_arc_create(lv_scr_act());                                             /* 创建圆弧 */
    lv_obj_set_size(arc_left, scr_act_height() * 3/8, scr_act_height() * 3/8);          /* 设置大小 */
    lv_obj_align(arc_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);                     /* 设置位置 */
    lv_arc_set_value(arc_left, 0);                                                      /* 设置当前值 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_MAIN);                      /* 设置背景弧宽度 */
    lv_obj_set_style_arc_width(arc_left, arc_width, LV_PART_INDICATOR);                 /* 设置前景弧宽度 */
//    lv_obj_add_event_cb(arc_left, arc_event_cb, LV_EVENT_VALUE_CHANGED, NULL);          /* 添加事件 */

    /* 左侧百分比标签 */
    label_left = lv_label_create(lv_scr_act());                                         /* 创建百分比标签 */
    lv_obj_align(label_left, LV_ALIGN_CENTER, -scr_act_width()/5, 0);                   /* 设置位置 */
    lv_label_set_text(label_left, "33%");                                                /* 设置文本 */
}

static void UI_Thread(void const *argument)
{
  osEvent event = {0};

  /* 系统其他硬件初始化信息，这里忽略 */
  // btim_timx_int_init(10-1,7200-1); /* 根据自己的开发板 MCU 定义定时器周期为 1ms */
  lv_init(); /* lvgl 系统初始化 */
  lv_port_disp_init(); /* lvgl 显示接口初始化,放在 lv_init()的后面 */
//  lv_port_indev_init(); /* lvgl 输入接口初始化,放在 lv_init()的后面 */
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label,"Hello Alientek!!!");
  lv_obj_center(label);

  lv_example_arc1();
  while(1)
  {
    lv_timer_handler(); /* LVGL 管理函数相当于 RTOS 触发任务调度函数 */
    osDelay(5);
  }
}

void ui_init(void)
{
  osThreadDef(UIThread, UI_Thread, osPriorityAboveNormal, 0, 512);
  UI_ThreadHandle = osThreadCreate(osThread(UIThread), NULL);
}
