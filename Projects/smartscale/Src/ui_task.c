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

void ui_init(viod)
{
  draw_single(430, 0, 2);
  text_show_font_index(480/2-48, 5, 0, 48, 0, BLACK);//索引汉字
  text_show_font_index(480/2, 5, 1, 48, 0, BLACK);
  lcd_draw_hline(BLACK, 0, 60, 480);

  text_show_string(480/2, 65, 100, 48, "0.00", 48, 0, BLACK);
  text_show_string(480/2, 65 + 48 * 1, 100, 48, "0.00", 48, 0, BLACK);
  text_show_string(480/2, 65 + 48 * 2, 100, 48, "0.00", 48, 0, BLACK);
  text_show_string(480/2, 65 + 48 * 3, 100, 48, "0.00", 48, 0, GREEN);

  lcd_draw_hline(BLACK, 0, 65 + 48 * 4 + 5, 480);

  // text_show_string(480/2, 65 + 48 * 4 + 5 + 10, 100, 32, "0.00", 32, 0, BLACK);

  // text_show_font_index(0, 5+60, 0, 40, 0, BLACK);
  // text_show_font_index(48, 5+60, 1, 40, 0, BLACK);

  // text_show_string(480/2-40, 0, 80, 40, "����", 40, 0, RED);

  // text_show_string(128, 100, 160, 32, "���ӳ�", 32, 0, RED);
  // text_show_string(128, 200, 160, 40, "���ӳ�", 40, 0, RED);

  // text_show_font_index(128, 150, 2, 40, 0, RED);
  // text_show_font_index(128+40, 150, 3, 40, 0, RED);
  // text_show_font_index(128+40+40, 150, 4, 40, 0, RED);
}

static float weigth = 0;
static uint8_t level = 0;
void ui_task_handle(void)
{
    uint8_t test_buf[64];
    weigth += (float)HAL_GetTick() / 1000.0f;

    snprintf(test_buf, sizeof(test_buf), "%2.2f", weigth);
    text_show_string(480/2, 65, 100, 48, test_buf, 48, 0, BLACK);
    text_show_string(480/2, 65 + 48 * 1, 100, 48, test_buf, 48, 0, BLACK);
    text_show_string(480/2, 65 + 48 * 2, 100, 48, test_buf, 48, 0, BLACK);
    text_show_string(480/2, 65 + 48 * 3, 100, 48, test_buf, 48, 0, GREEN);

    level++;
    draw_single(430, 0, level % 4);
}
