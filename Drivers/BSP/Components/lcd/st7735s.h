#ifndef _ST7735S_H
#define _ST7735S_H

// #include "stm32f1xx.h"
#include <stdbool.h>
#include <stdint.h>
// #include "sys.h"
// #include "stdlib.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0 || USE_HORIZONTAL==1
#define  ST7735S_LCD_PIXEL_WIDTH    ((uint16_t)128)
#define  ST7735S_LCD_PIXEL_HEIGHT   ((uint16_t)160)
#else
#define  ST7735S_LCD_PIXEL_WIDTH    ((uint16_t)160)
#define  ST7735S_LCD_PIXEL_HEIGHT   ((uint16_t)128)
#endif


/**
  * @brief  ST7735S Size
  */


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


//LCD重要参数集
typedef struct
{
	uint16_t width;			    //LCD 宽度
	uint16_t height;			//LCD 高度
	uint16_t id;				//LCD ID
	uint8_t	wramcmd;		//开始写gram指令
	uint8_t  setxcmd;		//设置x坐标指令
	uint8_t  setycmd;		//设置y坐标指令
}_lcd_dev;

//LCD参数
extern _lcd_dev st7735s_dev;	//管理LCD重要参数

//画笔颜色
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
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


// void LCD_Init(void);													   	//初始化
// void LCD_DisplayOn(void);													//开显示
// void LCD_DisplayOff(void);													//关显示
// void lcd_clear(uint16_t Color);	 												//清屏
// void LCD_DrawPoint(uint16_t x,uint16_t y);											//画点
// void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);								//快速画点
// void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);										//画圆
// void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);							//画线
// void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   				//画矩形
// void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);		   				//填充单色
// void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);				//填充指定颜色
// void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);						//显示一个字符
// void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);  						//显示一个数字
// void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);				//显示 数字
// void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);		//显示一个字符串,12/16字体

// void showimage(uint16_t x,uint16_t y); //显示40*40图片


// void showhanzi16(unsigned int x,unsigned int y,unsigned char index);//16*16汉字
// void showhanzi32(unsigned int x,unsigned int y,unsigned char index);//32*32汉字

void st7735s_Init(void);
void st7735s_Write_Gram(uint16_t RGBCode);
void st7735s_SetCursor(uint16_t Xpos, uint16_t Ypos);
void st7735s_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);

void st7735s_test(void);

#ifdef __cplusplus
 }
#endif

#endif /* _ST7735S_H */