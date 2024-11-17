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
#include "sys_task.h"
#include "wl_task.h"
#include "cmsis_os.h"
#include "lcd.h"
#include "hx711.h"
#include "hot.h"
#include "system_info.h"
#include "wl_priv_data.h"


#define DISH_SIZE            (48)
#define DISH_WIDTH           (DISH_SIZE*6)
#define DISH_XPOST           (480/2-(3*DISH_SIZE))
#define DISH_YPOST           (5)

#define DEVICE_NUM_SIZE          (28)
#define DEVICE_NUM_WIDTH         (DEVICE_NUM_SIZE*2)
#define DEVICE_NUM_LINE_XPOST    (BIAOTI_LINE_XPOST/2 - DEVICE_NUM_SIZE)
#define DEVICE_NUM_LINE_YPOST    (65 + 48 * 3)

#define FIRST_LINE_YPOST        (65)
#define SECOUND_LINE_YPOST      (65 + 48 * 1)
#define THIRD_LINE_YPOST        (65 + 48 * 2)
#define FOURTH_LINE_YPOST       (65 + 48 * 3)

#define BIAOTI_SIZE             (28)
#define BIAOTI_WIDTH            (BIAOTI_SIZE*4 + BIAOTI_SIZE/2)
#define BIAOTI_LINE_XPOST       (480/2-(4*BIAOTI_SIZE+BIAOTI_SIZE/4))

#define NUM_SIZE                (48)
#define NUM_WIDTH               (48/2*6)
#define NUM_LINE_XPOST          (480/2+BIAOTI_SIZE/4)

#define UNIT_SIZE               (24)
#define UNIT_WIDTH              (24*4)//(24/2*7)
#define UNIT_LINE_XPOST         (390)

#define DOWN_SIZE               (28)
#define DOWN_LINE_YPOST         (280)

osThreadId UI_ThreadHandle;

const caiping_data_t default_caiping_data = {
  .dish_str = UI_DEFAULT_CAIPING,
  .mode = 0,
  .price = 0.30,
  .price_unit = 100,
  .tool_weight = 50,
  .zhendongwucha = 5,
  .devicenum = 12,
};

static char disp_str[64] = {0};
caiping_data_t caiping_data = {0};

#if DISPLAY_DEBUG_INFO
static osTimerId debug_timehandle = NULL;

static void debug_ostimercallback(void const * argument)
{
    (void) argument;
    ui_ossignal_notify(UI_NOTIFY_DEBUGINFO_BIT);
}
#endif

static void draw_single(uint16_t x, uint16_t y, uint8_t level)
{
  // lcd_draw_fill(RED, x, y, x + 40, y + 40);
  lcd_draw_fill(level > 0 ? BLACK : LGRAY, x + 6, y + 34, x + 10, y + 46);
  lcd_draw_fill(level > 1 ? BLACK : LGRAY, x + 16, y + 26, x + 20, y + 46);
  lcd_draw_fill(level > 2 ? BLACK : LGRAY, x + 26, y + 18, x + 30, y + 46);
  lcd_draw_fill(level > 3 ? BLACK : LGRAY, x + 36, y + 10 , x + 40, y + 46);
}

int32_t ui_ossignal_notify(int32_t signals)
{
    if(!UI_ThreadHandle)
    {
        return -1;
    }

    return osSignalSet(UI_ThreadHandle, signals);
}

static void UI_Thread(void const *argument)
{
  osEvent event = {0};

  osSignalSet(UI_ThreadHandle, UI_NOTIFY_ALL_BIT); // update all
#if DISPLAY_DEBUG_INFO
  osTimerStart(debug_timehandle, DEBUG_INFO_TIME);
#endif

  while(1) 
  {
    event = osSignalWait(UI_TASK_NOTIFY, osWaitForever);
    if(event.status == osEventSignal)
    {
      if(event.value.signals & UI_NOTIFY_LOCK_BIT)
      {
        text_show_logo(20, 120);

        //draw lines
        lcd_draw_hline(BLACK, 5, 60, 480-5*2);
        lcd_draw_hline(BLACK, 5, 265, 480-5*2);
        lcd_draw_vline(BLACK, 240, 265 + 5, 55-5*2);

        // first line
        text_show_string(BIAOTI_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, // text 单价
                        BIAOTI_WIDTH, BIAOTI_SIZE, 
                        UI_DANJIA_STR, 
                        BIAOTI_SIZE, 
                        0, 
                        BLACK);
        text_show_string(BIAOTI_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, // text 重量 
                        BIAOTI_WIDTH, BIAOTI_SIZE, 
                        UI_ZHONGLIANG_STR, 
                        BIAOTI_SIZE, 
                        0, 
                        BLACK);
        text_show_string(BIAOTI_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, // text 总价 
                        BIAOTI_WIDTH, BIAOTI_SIZE, 
                        UI_ZONGJIA_STR, 
                        BIAOTI_SIZE, 
                        0, 
                        BLACK);
        text_show_string(BIAOTI_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, // text 消费总额
                        BIAOTI_WIDTH, BIAOTI_SIZE, 
                        UI_XFZE_STR, 
                        BIAOTI_SIZE, 
                        0, 
                        BLACK);
        text_show_string(UNIT_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), // text 元
                        UNIT_WIDTH, UNIT_SIZE, 
                        UI_YUAN_STR, 
                        UNIT_SIZE, 
                        0, 
                        BLACK);
        text_show_string(UNIT_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), // text 元
                        UNIT_WIDTH, UNIT_SIZE, 
                        UI_YUAN_STR, 
                        UNIT_SIZE, 
                        0, 
                        BLACK);
#if DISPLAY_DEBUG_INFO
        snprintf(disp_str, sizeof(disp_str), "v:%s", MCU_FW_VERSION);
        text_show_string_left(0, 0, 12*6, 12, disp_str, 12, 0, BLUE);
#endif
      }

      if(event.value.signals & UI_NOTIFY_SIGNEL_BIT)
      {
        if(wl_get_status_bit(WL_STATUS_CGREG_BIT))
          snprintf(disp_str, sizeof(disp_str), "4G", get_sys_status());
        else
          snprintf(disp_str, sizeof(disp_str), "  ", get_sys_status());
        text_show_string_left(430+6, 10, 12, 12, disp_str, 12, 0, BLACK);

        // draw single
        uint8_t rssi = 0;
        if(wl.rssi == 0)
          rssi = 1;
        else if(wl.rssi == 1)
          rssi = 2;
        else if(wl.rssi <= 30)
          rssi = 3;
        else if(wl.rssi == 31) 
          rssi = 4;

        draw_single(430, 0, rssi);
      }

      if(event.value.signals & UI_NOTIFY_DISH_BIT)
      {
        text_show_string_middle(DISH_XPOST, DISH_YPOST, // text 菜品
                                DISH_WIDTH, DISH_SIZE, 
                                caiping_data.dish_str, 
                                DISH_SIZE, 
                                0, 
                                BLACK);   
      }

      if(event.value.signals & UI_NOTIFY_PRICE_BIT)
      {
        snprintf(disp_str, sizeof(disp_str), "%.2f", caiping_data.price); //text 单价
        text_show_string_left(NUM_LINE_XPOST, FIRST_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          disp_str, 
                          NUM_SIZE, 
                          0, 
                          BLACK);
      }

      if(event.value.signals & UI_NOTIFY_PRICE_UNIT_BIT)
      {
        //"元\/100g"
        snprintf(disp_str, sizeof(disp_str), "%s/%ldg", UI_YUAN_STR, caiping_data.price_unit); // text 单价单位
        text_show_string_left(UNIT_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
                              UNIT_WIDTH, UNIT_SIZE, 
                              disp_str, 
                              UNIT_SIZE, 
                              0, 
                              BLACK);
      }

      if(event.value.signals & UI_NOTIFY_WEIGHT_BIT)
      {
        int weight = abs(get_change_weight());
        snprintf(disp_str, sizeof(disp_str), "%d", weight);          
        text_show_string_left(NUM_LINE_XPOST, SECOUND_LINE_YPOST,       // text 称重重量
                              NUM_WIDTH, NUM_SIZE, 
                              disp_str, 
                              NUM_SIZE, 
                              0, 
                              BLACK);
      }

      if(event.value.signals & UI_NOTIFY_WEIGHT_UNIT_BIT)
      {
        text_show_string(UNIT_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-24), // text 重量单位
                          UNIT_WIDTH, UNIT_SIZE, 
                          UI_KE_STR, 
                          UNIT_SIZE, 
                          0, 
                          BLACK);
      }

      if(event.value.signals & UI_NOTIFY_SUM_PRICE_BIT)
      {
        int weight = abs(get_change_weight());
        float sum_price = sum_price = (float)weight * caiping_data.price;
        if(caiping_data.price_unit)
          sum_price = sum_price / caiping_data.price_unit;
        snprintf(disp_str, sizeof(disp_str), "%.2f", sum_price);                 
        text_show_string_left(NUM_LINE_XPOST, THIRD_LINE_YPOST,            // text 总价
                              NUM_WIDTH, NUM_SIZE, 
                              disp_str, 
                              NUM_SIZE, 
                              0, 
                              BLUE);
      }

      if(event.value.signals & UI_NOTIFY_SUMSUM_PRICE_BIT)
      {
        int weight = abs(get_change_weight());
        float sum_price = (float)weight * caiping_data.price;
        if(caiping_data.price_unit)
          sum_price = sum_price / caiping_data.price_unit;

        if(get_sys_status() == SYS_STATUS_QQC)
          sum_price += wlpriv.user_sumprice;
        snprintf(disp_str, sizeof(disp_str), "%.2f", sum_price);
        text_show_string_left(NUM_LINE_XPOST, FOURTH_LINE_YPOST,          // text 消费总额
                              NUM_WIDTH, NUM_SIZE, 
                              disp_str, 
                              NUM_SIZE, 
                              0, 
                              BLACK);
      }
      if(event.value.signals & UI_NOTIFY_DEVICENUM_BIT)
      {
        //system number width 4
        snprintf(disp_str, sizeof(disp_str), "%ld", caiping_data.devicenum);
        text_show_string_middle(DEVICE_NUM_LINE_XPOST, DEVICE_NUM_LINE_YPOST, // text 设备编号
                                DEVICE_NUM_WIDTH, DEVICE_NUM_SIZE, 
                                disp_str, 
                                DEVICE_NUM_SIZE, 
                                0, 
                                RED);
      }

      if(event.value.signals & UI_NOTIFY_STATUS_BIT)
      {
        //left down width 8
        uint8_t status = get_sys_status();
        LOG_I("device status:%s\r\n", ld_str[status]);
        text_show_string_middle((480/4*1-DOWN_SIZE/2*6), DOWN_LINE_YPOST,  // text status
                                DOWN_SIZE/2*12, DOWN_SIZE, 
                                ld_str[status], 
                                DOWN_SIZE, 
                                0, 
                                (status == SYS_STATUS_SBZC ? BLUE : RED));
      }

      if(event.value.signals & UI_NOTIFY_USERNUM_BIT)
      {
        //right down width 10
        uint8_t status = get_sys_status();
        snprintf(disp_str, sizeof(disp_str), "%s", (status == SYS_STATUS_QQC || status == SYS_STATUS_QBDCP) ? mj_str : UI_HYSY_STR);
        text_show_string_middle((480/4*3-DOWN_SIZE/2*5), DOWN_LINE_YPOST, // 扫码的编码，或者扫码识别的编码
                                DOWN_SIZE/2*10, DOWN_SIZE, 
                                disp_str, 
                                DOWN_SIZE, 
                                0, 
                                RED);
      }

#if DISPLAY_DEBUG_INFO
      if(event.value.signals & UI_NOTIFY_DEBUGINFO_BIT)
      {
        snprintf(disp_str, sizeof(disp_str), "w:%ldg, h:%d", hx711_get_weight_value(), get_current_hot_status());
        text_show_string_left(0, 12, 12*6, 12, disp_str, 12, 0, BLUE);

        snprintf(disp_str, sizeof(disp_str), "wl:%lx,%d", wl.status, wl.cme_error);
        text_show_string_left(0, 24, 12*8, 12, disp_str, 12, 0, BLUE);

        snprintf(disp_str, sizeof(disp_str), "pr t:%x,r:%d", wlpriv.tx_pnum, wlpriv.rx_pnum);
        text_show_string_left(0, 36, 12*8, 12, disp_str, 12, 0, BLUE);

        // snprintf(disp_str, sizeof(disp_str), "sys:%d", get_sys_status());
        // text_show_string_left(0, 48, 12*6, 12, disp_str, 12, 0, BLUE);
      }
#endif
    }
  }
}

void ui_init(void)
{
  caiping_data_t *store = sysinfo_get_caipin();
  memcpy(&caiping_data, store, sizeof(caiping_data_t));

#if DISPLAY_DEBUG_INFO
  osTimerDef(debug_timer, debug_ostimercallback);
  debug_timehandle = osTimerCreate(osTimer(debug_timer), osTimerPeriodic, NULL);
  assert_param(debug_timehandle);
#endif

  osThreadDef(UIThread, UI_Thread, osPriorityAboveNormal, 0, 512);
  UI_ThreadHandle = osThreadCreate(osThread(UIThread), NULL);
}
