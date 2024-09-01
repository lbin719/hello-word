#include "fs.h"
#include "lcd.h"
#include "fonts.h"
#include "text.h"
#include "ulog.h"


#define CAIMING_SIZE            (48)
#define CAIMING_WIDTH           (CAIMING_SIZE*6)
#define CAIMING_XPOST           (480/2-(3*CAIMING_SIZE))
#define CAIMING_YPOST           (5)

#define SYSTEMNUM_SIZE          (28)
#define SYSTEMNUM_WIDTH         (SYSTEMNUM_SIZE*2)
#define SYSTEMNUM_LINE_XPOST    (BIAOTI_LINE_XPOST/2 - SYSTEMNUM_SIZE)
#define SYSTEMNUM_LINE_YPOST    (65 + 48 * 3)

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

#define DANWEI_SIZE             (24)
#define DANWEI_WIDTH            (24*4)
#define DANWEI_LINE_XPOST       (390)


#define DOWN_SIZE               (28)
#define DOWN_LINE_YPOST         (280)

char *const leftdown_string[] =
{ 
  "设备离线", 
  "设备正常",
  "正在登陆",
};


void draw_single(uint16_t x, uint16_t y, uint8_t level)
{
  // lcd_draw_fill(RED, x, y, x + 40, y + 40);
  lcd_draw_fill(level > 0 ? BLACK : LGRAY, x + 6, y + 25, x + 10, y + 40);
  lcd_draw_fill(level > 1 ? BLACK : LGRAY, x + 16, y + 20, x + 20, y + 40);
  lcd_draw_fill(level > 2 ? BLACK : LGRAY, x + 26, y + 15, x + 30, y + 40);
  lcd_draw_fill(level > 3 ? BLACK : LGRAY, x + 36, y + 10 , x + 40, y + 40);
}


void ui_init(void)
{
    // draw single
    draw_single(430, 0, 2);

    //draw lines
    lcd_draw_hline(BLACK, 5, 60, 480-5*2);
    lcd_draw_hline(BLACK, 5, 265, 480-5*2);
    lcd_draw_vline(BLACK, 240, 265 + 5, 55-5*2);

    // draw name
    text_show_string_middle(CAIMING_XPOST, CAIMING_YPOST, 
                            CAIMING_WIDTH, CAIMING_SIZE, 
                            "小蘑菇炖土鸡", 
                            48, 
                            0, 
                            BLACK);    

    // first line
    text_show_string(BIAOTI_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                     BIAOTI_WIDTH, BIAOTI_SIZE, 
                     "单    价:", 
                     BIAOTI_SIZE, 
                     0, 
                     BLACK);
    text_show_string_left(NUM_LINE_XPOST, FIRST_LINE_YPOST, 
                     NUM_WIDTH, NUM_SIZE, 
                     "000.00", 
                     NUM_SIZE, 
                     0, 
                     BLACK);
    text_show_string(DANWEI_LINE_XPOST, FIRST_LINE_YPOST+(NUM_SIZE-DANWEI_SIZE), 
                     DANWEI_WIDTH, DANWEI_SIZE, 
                     "元\/100g", 
                     DANWEI_SIZE, 
                     0, 
                     BLACK);

    // second line
    text_show_string(BIAOTI_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                     BIAOTI_WIDTH, BIAOTI_SIZE, 
                     "重    量:", 
                     BIAOTI_SIZE, 
                     0, 
                     BLACK);
    text_show_string_left(NUM_LINE_XPOST, SECOUND_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          "000", 
                          NUM_SIZE, 
                          0, 
                          BLACK);
    text_show_string(DANWEI_LINE_XPOST, SECOUND_LINE_YPOST+(NUM_SIZE-24), DANWEI_WIDTH, 24, "克(g)", 24, 0, BLACK);


    // third line
    text_show_string(BIAOTI_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                     BIAOTI_WIDTH, BIAOTI_SIZE, 
                     "总    价:", 
                     BIAOTI_SIZE, 
                     0, 
                     BLACK);
    text_show_string_left(NUM_LINE_XPOST, THIRD_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          "009.00", 
                          NUM_SIZE, 
                          0, 
                          BLACK);
    text_show_string(DANWEI_LINE_XPOST, THIRD_LINE_YPOST+(NUM_SIZE-DANWEI_SIZE), 
                     DANWEI_WIDTH, DANWEI_SIZE, 
                     "元", 
                     DANWEI_SIZE, 
                     0, 
                     BLACK);

    // fourth line
    text_show_string(BIAOTI_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-BIAOTI_SIZE)/2, 
                     BIAOTI_WIDTH, BIAOTI_SIZE, 
                     "消费总额:", 
                     BIAOTI_SIZE, 
                     0, 
                     BLACK);
    text_show_string_left(NUM_LINE_XPOST, FOURTH_LINE_YPOST, 
                          NUM_WIDTH, NUM_SIZE, 
                          "99.99", 
                          NUM_SIZE, 
                          0, 
                          BLACK);
    text_show_string(DANWEI_LINE_XPOST, FOURTH_LINE_YPOST+(NUM_SIZE-DANWEI_SIZE), 
                     DANWEI_WIDTH, DANWEI_SIZE, 
                     "元", 
                     DANWEI_SIZE, 
                     0, 
                     BLACK);

    //system number width 4
    text_show_string_middle(SYSTEMNUM_LINE_XPOST, SYSTEMNUM_LINE_YPOST, 
                            SYSTEMNUM_WIDTH, SYSTEMNUM_SIZE, 
                            "1234", 
                            SYSTEMNUM_SIZE, 
                            0, 
                            RED);

    //left down width 8
    text_show_string_middle((480/4*1-DOWN_SIZE/2*4), DOWN_LINE_YPOST, 
                            DOWN_SIZE/2*8, DOWN_SIZE, 
                            "设备正常", 
                            DOWN_SIZE, 
                            0, 
                            BLACK);

    //right down width 10
    text_show_string_middle((480/4*3-DOWN_SIZE/2*5), DOWN_LINE_YPOST, 
                            DOWN_SIZE/2*10, DOWN_SIZE, 
                            "5845845871", 
                            DOWN_SIZE, 
                            0, 
                            RED);

}

static float weigth = 0;
static uint8_t level = 0;
void ui_task_handle(void)
{
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
