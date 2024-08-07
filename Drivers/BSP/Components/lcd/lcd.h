#ifndef _LCD_H
#define _LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define LCD_DRIVER_IC_ST7735S   0
#define LCD_DRIVER_IC_ILI9488   1
#define LCD_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define LCD_RST_HIGH()      (LCD_RST_GPIO_PORT->BSRR = LCD_RST_GPIO_PIN)
#define LCD_RST_LOW()       (LCD_RST_GPIO_PORT->BSRR = (uint32_t)(LCD_RST_GPIO_PIN  << 16u))

#define LCD_DC_HIGH()      (LCD_DC_GPIO_PORT->BSRR = LCD_DC_GPIO_PIN)
#define LCD_DC_LOW()       (LCD_DC_GPIO_PORT->BSRR = (uint32_t)(LCD_DC_GPIO_PIN  << 16u))

#define LCD_CS_HIGH()      (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN)
#define LCD_CS_LOW()       (LCD_CS_GPIO_PORT->BSRR = (uint32_t)(LCD_CS_GPIO_PIN  << 16u))


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
  // void     (*SetCursor)(uint16_t, uint16_t);
  // void     (*WritePixel)(uint16_t, uint16_t, uint16_t);
  // uint16_t (*ReadPixel)(uint16_t, uint16_t);

  //  /* Optimized operation */
  // void     (*SetDisplayWindow)(uint16_t, uint16_t, uint16_t, uint16_t);
  // void     (*DrawHLine)(uint16_t, uint16_t, uint16_t, uint16_t);
  // void     (*DrawVLine)(uint16_t, uint16_t, uint16_t, uint16_t);

  // uint16_t (*GetLcdPixelWidth)(void);
  // uint16_t (*GetLcdPixelHeight)(void);
  // void     (*DrawBitmap)(uint16_t, uint16_t, uint8_t*);
  // void     (*DrawRGBImage)(uint16_t, uint16_t, uint16_t, uint16_t, uint8_t*);
  // void     (*DrawBitLine16BPP)(uint16_t, uint16_t, uint16_t*, uint16_t);

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
}LCD_DrvTypeDef;

typedef struct
{
	uint16_t width;			    //LCD 宽度
	uint16_t height;			  //LCD 高度
	uint16_t id;				    //LCD ID
	uint8_t	 wramcmd;		    //开始写gram指令
	uint8_t  setxcmd;		    //设置x坐标指令
	uint8_t  setycmd;		    //设置y坐标指令
}_lcd_dev;

//画笔颜色
// #define WHITE         	  0xFFFF
// #define BLACK         	  0x0000
// #define BLUE              0x001F
// #define BRED              0XF81F
// #define GRED 			        0XFFE0
// #define GBLUE			        0X07FF
// #define RED           	  0xF800
// #define MAGENTA       	  0xF81F
// #define GREEN         	  0x07E0
// #define CYAN          	  0x7FFF
// #define YELLOW        	  0xFFE0
// #define BROWN 			      0XBC40 //棕色
// #define BRRED 			      0XFC07 //棕红色
// #define GRAY  			      0X8430 //灰色
// //GUI颜色

// #define DARKBLUE      	  0X01CF	//深蓝色
// #define LIGHTBLUE      	  0X7D7C	//浅蓝色
// #define GRAYBLUE       	  0X5458  //灰蓝色
// //以上三色为PANEL的颜色

// #define LIGHTGREEN     	  0X841F //浅绿色
// //#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
// #define LGRAY             0XC618 //浅灰色(PANNEL),窗体背景色

// #define LGRAYBLUE         0XA651 //浅灰蓝色(中间层颜色)
// #define LBBLUE            0X2B12 //浅棕蓝色(选择条目的反色)
//画笔颜色
#define WHITE         	 0xFCFCFC
#define BLACK            0X000000
#define RED           	 0xFC0000
#define GREEN            0x00FC00
#define BLUE             0x0000FC

// typedef struct
// {
//   uint32_t TextColor;
//   uint32_t BackColor;
//   sFONT    *pFont;

// }LCD_DrawPropTypeDef;

// /**
//   * @brief  Point structures definition
//   */
// typedef struct
// {
//   int16_t X;
//   int16_t Y;

// }Point, * pPoint;

// /**
//   * @brief  Line mode structures definition
//   */
// typedef enum
// {
//   CENTER_MODE             = 0x01,    /*!< Center mode */
//   RIGHT_MODE              = 0x02,    /*!< Right mode  */
//   LEFT_MODE               = 0x03     /*!< Left mode   */

// }Line_ModeTypdef;

// /**
//   * @}
//   */

// /** @defgroup STM32_ADAFRUIT_LCD_Exported_Constants
//   * @{
//   */

// #define __IO    volatile

// /**
//   * @brief  LCD status structure definition
//   */
// #define LCD_OK         0x00
// #define LCD_ERROR      0x01
// #define LCD_TIMEOUT    0x02

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

// /**
//   * @brief LCD default font
//   */
// #define LCD_DEFAULT_FONT         Font8

// /**
//   * @}
//   */

// /** @defgroup STM32_ADAFRUIT_LCD_Exported_Functions
//   * @{
//   */
// uint8_t  BSP_LCD_Init(void);
// uint32_t BSP_LCD_GetXSize(void);
// uint32_t BSP_LCD_GetYSize(void);

// uint16_t BSP_LCD_GetTextColor(void);
// uint16_t BSP_LCD_GetBackColor(void);
// void     BSP_LCD_SetTextColor(__IO uint16_t Color);
// void     BSP_LCD_SetBackColor(__IO uint16_t Color);
// void     BSP_LCD_SetFont(sFONT *fonts);
// sFONT    *BSP_LCD_GetFont(void);

// void     BSP_LCD_Clear(uint16_t Color);
// void     BSP_LCD_ClearStringLine(uint16_t Line);
// void     BSP_LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr);
// void     BSP_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Line_ModeTypdef Mode);
// void     BSP_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

// void     BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code);
// void     BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
// void     BSP_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
// void     BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
// void     BSP_LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
// void     BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
// void     BSP_LCD_DrawPolygon(pPoint Points, uint16_t PointCount);
// void     BSP_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
// void     BSP_LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pBmp);
// void     BSP_LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
// void     BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
// void     BSP_LCD_FillPolygon(pPoint Points, uint16_t PointCount);
// void     BSP_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);

// void     BSP_LCD_DisplayOff(void);
// void     BSP_LCD_DisplayOn(void);

extern _lcd_dev lcd_dev;
extern uint32_t g_back_color;

void lcd_write_data(uint8_t *data, uint16_t len);
void lcd_write_cmddata(uint8_t *data, uint16_t len);
void lcd_read_cmddata(uint8_t cmd, uint8_t *data, uint16_t len);
#if LCD_DRIVER_IC_ST7735S
void lcd_wr_data(uint16_t RGBCode);
#else
void lcd_wr_data(uint32_t RGBCode);
#endif
void lcd_panel_exec_cmd(const uint8_t *cmd_table, uint32_t len);

void lcd_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _LCD_H */

