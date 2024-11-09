#include "st7796.h"
#include "lcd.h"

// static uint8_t framebuff[ST7796_LCD_PIXEL_WIDTH * 2 * 80];

_lcd_dev st7796_dev = {
	.width = ST7796_LCD_PIXEL_WIDTH,
	.height = ST7796_LCD_PIXEL_HEIGHT,
    .id = 0x0000,
    .wramcmd = 0x2C,
    .setxcmd = 0x2A,
    .setycmd = 0x2B
};

void st7796_DisplayOn(void)
{

}

void st7796_DisplayOff(void)
{

}

static const uint8_t st7796_init_cmd[] =
{
    DISP_WR_CMD(0x11),
    DISP_DLY_MS(120),
    DISP_WR_CMD(0xf0, 0xc3),
    DISP_WR_CMD(0xf0, 0x96),
    DISP_WR_CMD(0x36, 0x48),
    DISP_WR_CMD(0x3A, 0x55),
    DISP_WR_CMD(0xB4, 0x01),
    DISP_WR_CMD(0xB7, 0xC6),
    DISP_WR_CMD(0xe8, 0x40, 0x8a, 0x00, 0x00, 0x29, 0x19, 0xa5, 0x33),
    DISP_WR_CMD(0xc1, 0x06),
    DISP_WR_CMD(0xc2, 0xa7),
    DISP_WR_CMD(0xc5, 0x18),

    //dir:方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转
#if LCD_HORIZONTAL == 0
    DISP_WR_CMD(0x36, (1<<3)|(0<<7)|(1<<6)|(0<<5)),
#elif LCD_HORIZONTAL == 1
    DISP_WR_CMD(0x36, (1<<3)|(1<<7)|(0<<6)|(0<<5)),
#elif LCD_HORIZONTAL == 2
    DISP_WR_CMD(0x36, (1<<3)|(1<<7)|(1<<6)|(1<<5)),
#elif LCD_HORIZONTAL == 3
    DISP_WR_CMD(0x36, (1<<3)|(0<<7)|(0<<6)|(1<<5)),
#endif
    DISP_WR_CMD(0xe0, 0xf0, 0x09, 0x0b, 0x06, 0x04, 0x15, 0x2f, 0x54, 0x42, 0x3c, 0x17, 0x14, 0x18, 0x1b), //Positive Voltage Gamma Control
    DISP_WR_CMD(0xe1, 0xf0, 0x09, 0x0b, 0x06, 0x04, 0x03, 0x2d, 0x43, 0x42, 0x3b, 0x16, 0x14, 0x17, 0x1b), //Negative Voltage Gamma Control
    DISP_WR_CMD(0xf0, 0x3c),
    DISP_WR_CMD(0xf0, 0x69),
    DISP_DLY_MS(120),
    DISP_WR_CMD(0x29),
};

void st7796_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	// uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF)};
	// uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF)};
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF), (uint8_t)((st7796_dev.width - 1)>>8), (uint8_t)((st7796_dev.width - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF), (uint8_t)((st7796_dev.height - 1)>>8), (uint8_t)((st7796_dev.height - 1)&0XFF)};

	lcd_write_cmddata(setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmddata(setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));

	uint8_t write_gram_cmd[] = {0x2C};
	lcd_write_cmddata(write_gram_cmd, sizeof(write_gram_cmd));
}

void st7796_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  if((Xpos >= st7796_dev.width) || (Ypos >= st7796_dev.height))
  {
    return;
  }

  /* Set Cursor */
  st7796_SetCursor(Xpos, Ypos);

	uint8_t data[] = {RGBCode >> 8, RGBCode};
	lcd_write_data(data, sizeof(data));
}

void st7796_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF), (uint8_t)((Xpos + Width - 1)>>8), (uint8_t)((Xpos + Width - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF), (uint8_t)((Ypos + Height - 1)>>8), (uint8_t)((Ypos + Height - 1)&0XFF)};

	lcd_write_cmddata(setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmddata(setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));

	uint8_t write_gram_cmd[] = {0x2C};
	lcd_write_cmddata(write_gram_cmd, sizeof(write_gram_cmd));
}

void st7796_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Xpos + Length > ST7796_LCD_PIXEL_WIDTH) return;

  /* Set Cursor */
  st7796_SetCursor(Xpos, Ypos);

	uint8_t data[] = {RGBCode >> 8, RGBCode};
  for(counter = 0; counter < Length; counter++)
  {
//    ArrayRGB[counter] = RGBCode;
	  lcd_write_data(data, sizeof(data));
  }

//  lcd_write_data((uint8_t*)&ArrayRGB[0], Length * 2);
}


void st7796_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Ypos + Length >  st7796_dev.height) return;
  for(counter = 0; counter < Length; counter++)
  {
    st7796_WritePixel(Xpos, Ypos + counter, RGBCode);
  }
}

void st7796_Fill(uint16_t RGBCode, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  uint16_t counter = 0;

  if(sx + ex > st7796_dev.width) return;
  if(sy + ey > st7796_dev.height) return;

  /* Set Cursor */
  st7796_SetDisplayWindow(sx, sy, (ex - sx + 1), (ey - sy + 1));

	uint8_t data[] = {RGBCode >> 8, RGBCode};
	uint16_t Length = (ex - sx + 1) * (ey - sy + 1);
  for(counter = 0; counter < Length; counter++)
  {
    lcd_write_data(data, sizeof(data));
  }
}

void st7796_Fill_Date(const uint8_t *buf, uint32_t len, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
  if(sx + ex > st7796_dev.width) return;
  if(sy + ey > st7796_dev.height) return;

  /* Set Cursor */
  st7796_SetDisplayWindow(sx, sy, (ex - sx + 1), (ey - sy + 1));
  lcd_write_data(buf, len);
}

uint16_t st7796_GetLcdPixelWidth(void)
{
  return st7796_dev.width;
}


uint16_t st7796_GetLcdPixelHeight(void)
{
  return st7796_dev.height;
}

void st7796_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, size = 0;

  /* Read bitmap size */
  size = *(volatile uint16_t *) (pbmp + 2);
  size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(volatile uint16_t *) (pbmp + 10);
  index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;
  size = (size - index)/2;
  pbmp += index;

  /* Set GRAM write direction and BGR = 0 */
  /* Memory access control: MY = 0, MX = 1, MV = 0, ML = 0 */
  uint8_t write_cmd[] = {0x36, 0x40};
  lcd_write_cmddata(write_cmd, sizeof(write_cmd));

  /* Set Cursor */
  st7796_SetCursor(Xpos, Ypos);

  lcd_write_data((uint8_t*)pbmp, size*2);

  /* Set GRAM write direction and BGR = 0 */
  /* Memory access control: MY = 1, MX = 1, MV = 0, ML = 0 */
  write_cmd[1] = 0xC0;
  lcd_write_cmddata(write_cmd, sizeof(write_cmd));
}


void st7796_Clear(uint16_t color)
{
	st7796_SetCursor(0, 0);

	uint32_t totalpoint = st7796_dev.width * st7796_dev.height;
	uint8_t data[] = {color >> 8, color};
	for(uint32_t index = 0; index < totalpoint; index++)
	{
	  lcd_write_data(data, sizeof(data));
	}

	// for(uint32_t i = 0; i < sizeof(framebuff); i += 2)
	// {
	// 	framebuff[i] = color >> 8;
	// 	framebuff[i + 1] = color;
	// }

	// for(uint32_t index = 0; index < ST7796_LCD_PIXEL_HEIGHT/80; index++)
	// 	lcd_write_data(framebuff, sizeof(framebuff));
}


void st7796_Init(void)
{
	lcd_panel_exec_cmd(st7796_init_cmd, sizeof(st7796_init_cmd));
	st7796_SetDisplayWindow(0, 0, st7796_dev.width, st7796_dev.height);
  st7796_DisplayOn();
	st7796_Clear(WHITE);
}

LCD_DrvTypeDef   st7796_drv =
{
  .Init = st7796_Init,
  .ReadID = 0,
  .DisplayOn = st7796_DisplayOn,
  .DisplayOff = st7796_DisplayOff,
  .SetCursor = st7796_SetCursor,
  .WritePixel = st7796_WritePixel,
  .ReadPixel = 0,
  .SetDisplayWindow = st7796_SetDisplayWindow,
  .DrawHLine = st7796_DrawHLine,
  .DrawVLine = st7796_DrawVLine,
  .DrawFill = st7796_Fill,
  .DrawFillDate = st7796_Fill_Date,
  .GetLcdPixelWidth = st7796_GetLcdPixelWidth,
  .GetLcdPixelHeight = st7796_GetLcdPixelHeight,
  .DrawBitmap = st7796_DrawBitmap,
};

LCD_DrvTypeDef* st7796_probe(void)
{
	return &st7796_drv;
}
