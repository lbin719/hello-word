#include "fs.h"
#include "lcd.h"
#include "fonts.h"
#include "text.h"
#include "ulog.h"
#include "ui_task.h"
#include "version.h"
#include "gbk.h"
#include "application.h"
#include "cmsis_os.h"

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
#define UNIT_WIDTH              (24*4)
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
  .zhendongwucha = 15,
  .devicenum = 12,
};

static char disp_str[64] = {0};
caiping_data_t caiping_data = {0};

static void draw_single(uint16_t x, uint16_t y, uint8_t level)
{
  // lcd_draw_fill(RED, x, y, x + 40, y + 40);
  lcd_draw_fill(level > 0 ? BLACK : LGRAY, x + 6, y + 25, x + 10, y + 40);
  lcd_draw_fill(level > 1 ? BLACK : LGRAY, x + 16, y + 20, x + 20, y + 40);
  lcd_draw_fill(level > 2 ? BLACK : LGRAY, x + 26, y + 15, x + 30, y + 40);
  lcd_draw_fill(level > 3 ? BLACK : LGRAY, x + 36, y + 10 , x + 40, y + 40);
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

  while(1) 
  {
    event = osSignalWait(UI_TASK_NOTIFY, UI_TASK_DELAY);
    if(event.status == osEventSignal)
    {
      if(event.value.signals & UI_NOTIFY_LOCK_BIT)
      {
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
        char disp_str[32] = {0};
        snprintf(disp_str, sizeof(disp_str), "v:%s", MCU_FW_VERSION);
        text_show_string_left(0, 0, 12*6, 12, disp_str, 12, 0, BLUE);
#endif
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
        snprintf(disp_str, sizeof(disp_str), "%s\/%dg", UI_YUAN_STR, caiping_data.price_unit); // text 单价单位
        text_show_string(UNIT_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
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
        float sum_price = (float)weight * caiping_data.price / caiping_data.price_unit;
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
        float sum_price = (float)weight * caiping_data.price / caiping_data.price_unit;
        snprintf(disp_str, sizeof(disp_str), "%.2f", sum_price + 12);
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
        snprintf(disp_str, sizeof(disp_str), "%d", caiping_data.devicenum);
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
        // snprintf(disp_str, sizeof(disp_str), "%d", caiping_data.devicenum);
        text_show_string_middle((480/4*3-DOWN_SIZE/2*5), DOWN_LINE_YPOST, // 扫码的编码，或者扫码识别的编码
                                DOWN_SIZE/2*10, DOWN_SIZE, 
                                UI_HYSY_STR, 
                                DOWN_SIZE, 
                                0, 
                                RED);
      }

      if(event.value.signals & UI_NOTIFY_SIGNEL_BIT)
      {
        // draw single
        draw_single(430, 0, 2);
      }
    }
    else if(event.status == osEventTimeout)
    {
#if DISPLAY_DEBUG_INFO
      char disp_str[32] = {0};
      snprintf(disp_str, sizeof(disp_str), "w:%dg", hx711_get_weight_value());
      text_show_string_left(0, 12, 12*6, 12, disp_str, 12, 0, BLUE);

      snprintf(disp_str, sizeof(disp_str), "sys:%d", sys_status);
      text_show_string_left(0, 24, 12*6, 12, disp_str, 12, 0, BLUE);

      snprintf(disp_str, sizeof(disp_str), "wl:%d", wl.state);
      text_show_string_left(0, 36, 12*6, 12, disp_str, 12, 0, BLUE);
#endif
    }

  }
}

void ui_init(void)
{
  caiping_data_t *store = sysinfo_get_caipin();
  memcpy(&caiping_data, store, sizeof(caiping_data_t));

  osThreadDef(UIThread, UI_Thread, osPriorityAboveNormal, 0, 512);
  UI_ThreadHandle = osThreadCreate(osThread(UIThread), NULL);

  osDelay(500);
  osSignalSet(UI_ThreadHandle, UI_NOTIFY_ALL_BIT);
}