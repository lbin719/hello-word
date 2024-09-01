#include "fs.h"
#include "lcd.h"
#include "fonts.h"
#include "text.h"
#include "ulog.h"
#include "ui_task.h"


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

#define UNIT_SIZE             (24)
#define UNIT_WIDTH            (24*4)
#define UNIT_LINE_XPOST       (390)


#define DOWN_SIZE               (28)
#define DOWN_LINE_YPOST         (280)

char *const leftdown_string[] =
{ 
  "设备离线", 
  "设备正常",
  "正在登陆",
};

ui_draw_t ui_draw = {
  .dish_str = "萝卜炒肉",

  .price_str = "0.30",
  .price_unit_str = "元\/100g",

  .weight_str = "0",
  .weight_unit_str = "克(g)",

  .sum_price_str = "0.00",
  .sumsum_price_str = "0.00",

  .devicenum_str = "01",
  .status_str = "正在登录",
  .usernum_str = "欢迎使用",
};


volatile uint32_t draw_update_bit = DRAW_UPDATE_ALL_BIT;

void set_draw_update_bit(uint32_t data_bit)
{
  draw_update_bit |= data_bit;
}


void draw_single(uint16_t x, uint16_t y, uint8_t level)
{
  // lcd_draw_fill(RED, x, y, x + 40, y + 40);
  lcd_draw_fill(level > 0 ? BLACK : LGRAY, x + 6, y + 25, x + 10, y + 40);
  lcd_draw_fill(level > 1 ? BLACK : LGRAY, x + 16, y + 20, x + 20, y + 40);
  lcd_draw_fill(level > 2 ? BLACK : LGRAY, x + 26, y + 15, x + 30, y + 40);
  lcd_draw_fill(level > 3 ? BLACK : LGRAY, x + 36, y + 10 , x + 40, y + 40);
}

static void draw_update(void)
{
  if(draw_update_bit == 0)
    return 0;

  // LOG_I("draw_update:%08x\r\n", draw_update_bit);

  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_LOCK_BIT))
  {
    //draw lines
    lcd_draw_hline(BLACK, 5, 60, 480-5*2);
    lcd_draw_hline(BLACK, 5, 265, 480-5*2);
    lcd_draw_vline(BLACK, 240, 265 + 5, 55-5*2);

    // first line
    text_show_string(BIAOTI_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                      BIAOTI_WIDTH, BIAOTI_SIZE, 
                      "单    价:", 
                      BIAOTI_SIZE, 
                      0, 
                      BLACK);
    text_show_string(BIAOTI_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                      BIAOTI_WIDTH, BIAOTI_SIZE, 
                      "重    量:", 
                      BIAOTI_SIZE, 
                      0, 
                      BLACK);
    text_show_string(BIAOTI_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                      BIAOTI_WIDTH, BIAOTI_SIZE, 
                      "总    价:", 
                      BIAOTI_SIZE, 
                      0, 
                      BLACK);
    text_show_string(BIAOTI_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                      BIAOTI_WIDTH, BIAOTI_SIZE, 
                      "消费总额:", 
                      BIAOTI_SIZE, 
                      0, 
                      BLACK);

    text_show_string(UNIT_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
                      UNIT_WIDTH, UNIT_SIZE, 
                      "元", 
                      UNIT_SIZE, 
                      0, 
                      BLACK);
    text_show_string(UNIT_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
                      UNIT_WIDTH, UNIT_SIZE, 
                      "元", 
                      UNIT_SIZE, 
                      0, 
                      BLACK);
  }

  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_DISH_BIT))
  {
    text_show_string_middle(DISH_XPOST, DISH_YPOST, 
                            DISH_WIDTH, DISH_SIZE, 
                            ui_draw.dish_str, 
                            DISH_SIZE, 
                            0, 
                            BLACK);    
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_PRICE_BIT))
  {
    text_show_string_left(NUM_LINE_XPOST, FIRST_LINE_YPOST, 
                      NUM_WIDTH, NUM_SIZE, 
                      ui_draw.price_str, 
                      NUM_SIZE, 
                      0, 
                      BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_PRICE_UNIT_BIT))
  {
    text_show_string(UNIT_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
                      UNIT_WIDTH, UNIT_SIZE, 
                      ui_draw.price_unit_str, 
                      UNIT_SIZE, 
                      0, 
                      BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_WEIGHT_BIT))
  {
    text_show_string_left(NUM_LINE_XPOST, SECOUND_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          ui_draw.weight_str, 
                          NUM_SIZE, 
                          0, 
                          BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_WEIGHT_UNIT_BIT))
  {
    text_show_string(UNIT_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-24), 
                      UNIT_WIDTH, UNIT_SIZE, 
                      ui_draw.weight_unit_str, 
                      UNIT_SIZE, 
                      0, 
                      BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_SUM_PRICE_BIT))
  {
    text_show_string_left(NUM_LINE_XPOST, THIRD_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          ui_draw.sum_price_str, 
                          NUM_SIZE, 
                          0, 
                          BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_SUMSUM_PRICE_BIT))
  {
  text_show_string_left(NUM_LINE_XPOST, FOURTH_LINE_YPOST, 
                        NUM_WIDTH, NUM_SIZE, 
                        ui_draw.sumsum_price_str, 
                        NUM_SIZE, 
                        0, 
                        BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_DEVICENUM_BIT))
  {
    //system number width 4
    text_show_string_middle(DEVICE_NUM_LINE_XPOST, DEVICE_NUM_LINE_YPOST, 
                            DEVICE_NUM_WIDTH, DEVICE_NUM_SIZE, 
                            ui_draw.devicenum_str, 
                            DEVICE_NUM_SIZE, 
                            0, 
                            RED);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_STATUS_BIT))
  {
    //left down width 8
    text_show_string_middle((480/4*1-DOWN_SIZE/2*4), DOWN_LINE_YPOST, 
                            DOWN_SIZE/2*8, DOWN_SIZE, 
                            ui_draw.status_str, 
                            DOWN_SIZE, 
                            0, 
                            BLACK);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_USERNUM_BIT))
  {
    //right down width 10
    text_show_string_middle((480/4*3-DOWN_SIZE/2*5), DOWN_LINE_YPOST, 
                            DOWN_SIZE/2*10, DOWN_SIZE, 
                            ui_draw.usernum_str, 
                            DOWN_SIZE, 
                            0, 
                            RED);
  }
  if(draw_update_bit & (DRAW_UPDATE_ALL_BIT | DRAW_UPDATE_SIGNEL_BIT))
  {
    // draw single
    draw_single(430, 0, 2);
  }

  draw_update_bit = 0;
}

void ui_init(void)
{
  draw_update();
  // // draw single
  // draw_single(430, 0, 2);

  // text_show_string_middle(DISH_XPOST, DISH_YPOST, 
  //                         DISH_WIDTH, DISH_SIZE, 
  //                         ui_draw.dish_str, 
  //                         DISH_SIZE, 
  //                         0, 
  //                         BLACK);    



  // text_show_string_left(NUM_LINE_XPOST, FIRST_LINE_YPOST, 
  //                   NUM_WIDTH, NUM_SIZE, 
  //                   ui_draw.price_str, 
  //                   NUM_SIZE, 
  //                   0, 
  //                   BLACK);
  // text_show_string(UNIT_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-UNIT_SIZE), 
  //                   UNIT_WIDTH, UNIT_SIZE, 
  //                   ui_draw.price_unit_str, 
  //                   UNIT_SIZE, 
  //                   0, 
  //                   BLACK);

  // // second line

  // text_show_string_left(NUM_LINE_XPOST, SECOUND_LINE_YPOST, 
  //                       NUM_WIDTH, NUM_SIZE, 
  //                       ui_draw.weight_str, 
  //                       NUM_SIZE, 
  //                       0, 
  //                       BLACK);
  // text_show_string(UNIT_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-24), 
  //                   UNIT_WIDTH, UNIT_SIZE, 
  //                   ui_draw.weight_unit_str, 
  //                   UNIT_SIZE, 
  //                   0, 
  //                   BLACK);


  // // third line

  // text_show_string_left(NUM_LINE_XPOST, THIRD_LINE_YPOST, 
  //                       NUM_WIDTH, NUM_SIZE, 
  //                       ui_draw.sum_price_str, 
  //                       NUM_SIZE, 
  //                       0, 
  //                       BLACK);


  // // fourth line

  // text_show_string_left(NUM_LINE_XPOST, FOURTH_LINE_YPOST, 
  //                       NUM_WIDTH, NUM_SIZE, 
  //                       ui_draw.sumsum_price_str, 
  //                       NUM_SIZE, 
  //                       0, 
  //                       BLACK);


  // //system number width 4
  // text_show_string_middle(DEVICE_NUM_LINE_XPOST, DEVICE_NUM_LINE_YPOST, 
  //                         DEVICE_NUM_WIDTH, DEVICE_NUM_SIZE, 
  //                         ui_draw.devicenum_str, 
  //                         DEVICE_NUM_SIZE, 
  //                         0, 
  //                         RED);

  // //left down width 8
  // text_show_string_middle((480/4*1-DOWN_SIZE/2*4), DOWN_LINE_YPOST, 
  //                         DOWN_SIZE/2*8, DOWN_SIZE, 
  //                         ui_draw.status_str, 
  //                         DOWN_SIZE, 
  //                         0, 
  //                         BLACK);

  // //right down width 10
  // text_show_string_middle((480/4*3-DOWN_SIZE/2*5), DOWN_LINE_YPOST, 
  //                         DOWN_SIZE/2*10, DOWN_SIZE, 
  //                         ui_draw.usernum_str, 
  //                         DOWN_SIZE, 
  //                         0, 
  //                         RED);

}


static float weigth = 0;
static uint8_t level = 0;
void ui_task_handle(void)
{

  draw_update();
  return ;

  uint8_t test_buf[64];
  weigth += (float)HAL_GetTick() / 1000.0f;
  weigth = 125.4;
  snprintf(test_buf, sizeof(test_buf), "%.1f", weigth);
  // text_show_string(480/2, 65, 48*3, 48, test_buf, 48, 0, BLACK);
  // text_show_string(480/2, 65 + 48 * 1, 48*3, 48, test_buf, 48, 0, BLACK);
  // text_show_string(480/2, 65 + 48 * 2, 48*3, 48, test_buf, 48, 0, BLACK);
  // text_show_string(480/2, 65 + 48 * 3, 48*3, 48, test_buf, 48, 0, GREEN);

  level++;
  draw_single(430, 0, level % 4);
}
