#ifndef _ST7796_H
#define _ST7796_H

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if LCD_HORIZONTAL==0 || LCD_HORIZONTAL==1
#define  ST7796_LCD_PIXEL_WIDTH    ((uint16_t)320)
#define  ST7796_LCD_PIXEL_HEIGHT   ((uint16_t)480)
#else
#define  ST7796_LCD_PIXEL_WIDTH    ((uint16_t)480)
#define  ST7796_LCD_PIXEL_HEIGHT   ((uint16_t)320)
#endif


LCD_DrvTypeDef* st7796_probe(void);

#ifdef __cplusplus
 }
#endif

#endif /* _ST7796_H */