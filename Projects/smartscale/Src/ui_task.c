#include "fs.h"
#include "lcd.h"
#include "fonts.h"
#include "text.h"
#include "ulog.h"


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
    // draw name
    text_show_string_middle(480/2-(3*48), 5, 48*6, 48, "小蘑菇炖土鸡", 48, 0, BLACK);    

    // draw line
    draw_single(430, 0, 2);

    //draw lines
    lcd_draw_hline(BLACK, 0, 60, 480);
    lcd_draw_hline(BLACK, 0, 265, 480);
    lcd_draw_vline(BLACK, 240, 265, 55);

    // first line
    text_show_string(120, 65 + 12, 32*4, 32, "单  价:", 32, 0, BLACK);
    text_show_string(480/2, 65, 120, 48, "0.00", 48, 0, BLACK);
    text_show_string(360, 65 + 12, 120, 32, "元\/100g", 32, 0, BLACK);

    // second line
    text_show_string(120, (65 + 48 * 1 + 12), 32*4, 32, "重  量:", 32, 0, BLACK);
    text_show_string(480/2, (65 + 48 * 1), 100, 48, "0.00", 48, 0, BLACK);
    text_show_string(360, (65 + 48 * 1 + 12), 120, 32, "克(g)", 32, 0, BLACK);

    // third line
    text_show_string(120, (65 + 48 * 2 + 12), 32*4, 32, "总  价:", 32, 0, BLACK);
    text_show_string(480/2, (65 + 48 * 2), 100, 48, "0.00", 48, 0, BLACK);
    text_show_string(360, (65 + 48 * 2 + 12), 120, 32, "元", 32, 0, BLACK);

    // fourth line
    text_show_string(120 - 32, (65 + 48 * 3 + 12), 32*5, 32, "消费总额:", 32, 0, GREEN);
    text_show_string(480/2, (65 + 48 * 3), 100, 48, "0.00", 48, 0, GREEN);
    text_show_string(360, (65 + 48 * 3 + 12), 120, 32, "元", 32, 0, GREEN);

    //left down width 8
    text_show_string_middle((480/4*1-32/2*4), 280, 32/2*8, 32, "欢迎使用", 32, 0, RED);

    //right down width 10
    text_show_string_middle((480/4*3-32/2*5), 280, 32/2*10, 32, "zh123456", 32, 0, RED);

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
