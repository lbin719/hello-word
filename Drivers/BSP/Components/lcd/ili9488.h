#ifndef _ILI9488_H
#define _ILI9488_H

#include "lcd.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if LCD_HORIZONTAL==0 || LCD_HORIZONTAL==1
#define  ILI9488_LCD_PIXEL_WIDTH    ((uint16_t)320)
#define  ILI9488_LCD_PIXEL_HEIGHT   ((uint16_t)480)
#else
#define  ILI9488_LCD_PIXEL_WIDTH    ((uint16_t)480)
#define  ILI9488_LCD_PIXEL_HEIGHT   ((uint16_t)320)
#endif

// void ili9488_Init(void);
// void ili9488_Write_Gram(uint16_t RGBCode);
// void ili9488_SetCursor(uint16_t Xpos, uint16_t Ypos);
// void ili9488_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);

LCD_DrvTypeDef* ili9488_probe(void);

#ifdef __cplusplus
 }
#endif

#endif /* _ILI9488_H */