#ifndef _LCD_H
#define _LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define LCD_DRIVER_IC_ST7735S   0
#define LCD_DRIVER_IC_ST7796    1
#define LCD_DRIVER_IC_ILI9488   0

#define LCD_HORIZONTAL 3  //閻庤鐭粻鐔封槈閸欏鐝烽悘鐐茬箻閵嗗酣寮崼鏇熷珱闁哄啫顑堝ù鍡涘棘閻熺増鍊� 	0-0閹艰揪闄勫Λ鍡樻姜椤掑﹦绀�1-180閹艰揪闄勫Λ鍡樻姜椤掑﹦绀�2-270閹艰揪闄勫Λ鍡樻姜椤掑﹦绀�3-90閹艰揪闄勫Λ鍡樻姜閿燂拷

#if LCD_DRIVER_IC_ILI9488
#define LCD_PIXEL_RGB666
#endif


#define LCD_RST_HIGH()      (LCD_RST_GPIO_PORT->BSRR = LCD_RST_GPIO_PIN)
#define LCD_RST_LOW()       (LCD_RST_GPIO_PORT->BSRR = (uint32_t)(LCD_RST_GPIO_PIN  << 16u))

#define LCD_DC_HIGH()      (LCD_DC_GPIO_PORT->BSRR = LCD_DC_GPIO_PIN)
#define LCD_DC_LOW()       (LCD_DC_GPIO_PORT->BSRR = (uint32_t)(LCD_DC_GPIO_PIN  << 16u))

#define LCD_CS_HIGH()      (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN)
#define LCD_CS_LOW()       (LCD_CS_GPIO_PORT->BSRR = (uint32_t)(LCD_CS_GPIO_PIN  << 16u))

#define LCD_BLK_ENABLE()   (LCD_BLK_GPIO_PORT->BSRR = LCD_BLK_GPIO_PIN)
#define LCD_BLK_DISABLE()  (LCD_BLK_GPIO_PORT->BSRR = (uint32_t)(LCD_BLK_GPIO_PIN  << 16u))

typedef enum
{
    CMD_TYPE_WR_CMD,
    CMD_TYPE_DLY_MS,
} panel_cmd_type_e;

typedef enum
{
    CMD_IDX_TYPE   = 0,
    CMD_IDX_LEN    = 1,
    CMD_HEADER_LEN = 2,
    CMD_IDX_CODE   = 2,
} panel_cmd_idx_e;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))
#endif

#define DISP_WR_CMD(x, ...)  CMD_TYPE_WR_CMD, ARRAY_SIZE(((uint8_t[]) \
                                    {x, ##__VA_ARGS__})), x, ##__VA_ARGS__
#define DISP_DLY_MS(x, ...)  CMD_TYPE_DLY_MS, ARRAY_SIZE(((uint8_t[]) \
                                    {x, ##__VA_ARGS__})), x, ##__VA_ARGS__
#define DISP_CMD_ENTRY(x)    {.len = ARRAY_SIZE(x), .cmd = x}

#define CTRL_LIST_ENTRY(x)   {.len = ARRAY_SIZE(x), .list = x}

typedef struct
{
  void     (*Init)(void);
  uint16_t (*ReadID)(void);
  void     (*DisplayOn)(void);
  void     (*DisplayOff)(void);

#ifdef LCD_PIXEL_RGB666
  void     (*SetCursor)(uint16_t, uint16_t);
  void     (*WritePixel)(uint16_t, uint16_t, uint32_t);
  uint32_t (*ReadPixel)(uint16_t, uint16_t);

   /* Optimized operation */
  void     (*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);

  uint16_t (*GetLcdPixelWidth)(void);
  uint16_t (*GetLcdPixelHeight)(void);
  void     (*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
  void     (*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
  void     (*DrawBitLine16BPP)(uint16_t, uint16_t, uint16_t*, uint16_t);
#else
  void     (*SetCursor)(uint16_t, uint16_t);
  void     (*WritePixel)(uint16_t, uint16_t, uint16_t);
  uint16_t (*ReadPixel)(uint16_t, uint16_t);

   /* Optimized operation */
  void     (*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  void     (*DrawFill)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
  uint16_t (*GetLcdPixelWidth)(void);
  uint16_t (*GetLcdPixelHeight)(void);
  void     (*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
  void     (*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
  void     (*DrawBitLine16BPP)(uint16_t, uint16_t, uint16_t*, uint16_t);
#endif
}LCD_DrvTypeDef;

typedef struct
{
	uint16_t width;
	uint16_t height;
	uint16_t id;
	uint8_t	 wramcmd;
	uint8_t  setxcmd;
	uint8_t  setycmd;
}_lcd_dev;


#ifdef LCD_PIXEL_RGB666

#define WHITE         	 0xFCFCFC
#define BLACK            0X000000
#define RED           	 0xFC0000
#define GREEN            0x00FC00
#define BLUE             0x0000FC

#else

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY 			     0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

#endif


// /**
//   * @brief  LCD color
//   */
// #define LCD_COLOR_BLACK         0x0000
// #define LCD_COLOR_GREY          0xF7DE
// #define LCD_COLOR_BLUE          0x001F
// #define LCD_COLOR_RED           0xF800
// #define LCD_COLOR_GREEN         0x07E0
// #define LCD_COLOR_CYAN          0x07FF
// #define LCD_COLOR_MAGENTA       0xF81F
// #define LCD_COLOR_YELLOW        0xFFE0
// #define LCD_COLOR_WHITE         0xFFFF



extern _lcd_dev lcd_dev;
extern uint32_t g_back_color;

void lcd_write_data(uint8_t *data, uint16_t len);
void lcd_write_cmddata(uint8_t *data, uint16_t len);
void lcd_read_cmddata(uint8_t cmd, uint8_t *data, uint16_t len);

void lcd_panel_exec_cmd(const uint8_t *cmd_table, uint32_t len);


void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);
void lcd_draw_hline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void lcd_draw_fill(uint16_t RGBCode, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);
void lcd_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _LCD_H */

