#ifndef _ST7735S_H
#define _ST7735S_H

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"

#ifdef __cplusplus
 extern "C" {
#endif


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

// void st7735s_Init(void);
// void st7735s_Write_Gram(uint16_t RGBCode);
// void st7735s_SetCursor(uint16_t Xpos, uint16_t Ypos);
// void st7735s_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);

// void st7735s_test(void);
LCD_DrvTypeDef* st7735s_probe(void);

#ifdef __cplusplus
 }
#endif

#endif /* _ST7735S_H */