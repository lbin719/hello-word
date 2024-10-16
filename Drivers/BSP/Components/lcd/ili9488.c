#include "stm32f1xx_hal.h"
#include "ulog.h"
#include "main.h"
#include "spi.h"
#include "ili9488.h"
#include "lcd.h"


// static uint8_t framebuff[ILI9488_LCD_PIXEL_WIDTH * 2 * 80];

_lcd_dev ili9488_dev = {
	.width = ILI9488_LCD_PIXEL_WIDTH,
	.height = ILI9488_LCD_PIXEL_HEIGHT,
    .id = 0x0000,
    .wramcmd = 0x2C,
    .setxcmd = 0x2A,
    .setycmd = 0x2B
};

static const uint8_t ili9488_disp_on_cmd[] =
{
    DISP_WR_CMD(0x11),
    DISP_DLY_MS(120),
    DISP_WR_CMD(0x29),
};

static const uint8_t ili9488_disp_off_cmd[] =
{
    DISP_WR_CMD(0x28),
    DISP_WR_CMD(0x10),
    DISP_DLY_MS(120),
};

static const uint8_t ili9488_init_cmd[] =
{
	DISP_WR_CMD(0xE0, 0x00, 0x07, 0x0f, 0x0D, 0x1B, 0x0A, 0x3c, 0x78, 0x4A, 0x07, 0x0E, 0x09, 0x1B, 0x1e, 0x0f),
	DISP_WR_CMD(0xE1, 0x00, 0x22, 0x24, 0x06, 0x12, 0x07, 0x36, 0x47, 0x47, 0x06, 0x0a, 0x07, 0x30, 0x37, 0x0f),
	DISP_WR_CMD(0xC0, 0x10, 0x10),
	DISP_WR_CMD(0xC1, 0x41),
	DISP_WR_CMD(0xC5, 0x00, 0x22, 0x80),

#if LCD_HORIZONTAL==0 // Memory Access Control
	DISP_WR_CMD(0x36, 0x48),
#elif LCD_HORIZONTAL==1
	DISP_WR_CMD(0x36, 0x88),
#elif LCD_HORIZONTAL==2
	DISP_WR_CMD(0x36, 0x28),
#else
	DISP_WR_CMD(0x36, 0xE8),
#endif
	DISP_WR_CMD(0x3A, 0x66), //Interface Mode Control闁挎稑鐭夐幏锟�?闁跨喐鏋婚幏锟�?闁跨喓绁I9486闁跨噦鎷�??????0X55
	DISP_WR_CMD(0XB0, 0x00), //Interface Mode Control
	DISP_WR_CMD(0xB1, 0xB0, 0x11),    //Frame rate 70Hz
	DISP_WR_CMD(0xB4, 0x02),
	DISP_WR_CMD(0xB6, 0x02, 0x02),  //RGB/MCU Interface Control

	DISP_WR_CMD(0xB7, 0xC6),
	DISP_WR_CMD(0xE9, 0x00),
	DISP_WR_CMD(0XF7, 0xA9, 0x51, 0x2C, 0x82),
};

void ili9488_DisplayOn(void)
{
  lcd_panel_exec_cmd(ili9488_disp_on_cmd, sizeof(ili9488_disp_on_cmd));
}

void ili9488_DisplayOff(void)
{
  lcd_panel_exec_cmd(ili9488_disp_off_cmd, sizeof(ili9488_disp_off_cmd));
}

/**
  * @brief  Sets Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void ili9488_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	// uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF)};
	// uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF)};
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF), (uint8_t)((ili9488_dev.width - 1)>>8), (uint8_t)((ili9488_dev.width - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF), (uint8_t)((ili9488_dev.height - 1)>>8), (uint8_t)((ili9488_dev.height - 1)&0XFF)};

	lcd_write_cmddata(setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmddata(setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));

	uint8_t write_gram_cmd[] = {0x2C};
	lcd_write_cmddata(write_gram_cmd, sizeof(write_gram_cmd));
}

/**
  * @brief  Writes pixel.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color
  * @retval None
  */
void ili9488_WritePixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGBCode)
{
  if((Xpos >= ili9488_dev.width) || (Ypos >= ili9488_dev.height))
  {
    return;
  }

  /* Set Cursor */
  ili9488_SetCursor(Xpos, Ypos);

	uint8_t data[] = {RGBCode >> 16, RGBCode >> 8, RGBCode};
	lcd_write_data(data, sizeof(data));
}

void ili9488_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF), (uint8_t)((Xpos + Width - 1)>>8), (uint8_t)((Xpos + Width - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF), (uint8_t)((Ypos + Height - 1)>>8), (uint8_t)((Ypos + Height - 1)&0XFF)};

	lcd_write_cmddata(setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmddata(setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));
}

void ili9488_DrawHLine(uint32_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Xpos + Length > ili9488_dev.width) return;

  /* Set Cursor */
  ili9488_SetCursor(Xpos, Ypos);

  uint8_t data[] = {RGBCode >> 16, RGBCode >> 8, RGBCode};
  for(counter = 0; counter < Length; counter++)
  {
//    ArrayRGB[counter] = RGBCode;
	  lcd_write_data(data, sizeof(data));
  }

//  lcd_write_data((uint8_t*)&ArrayRGB[0], Length * 2);
}

void ili9488_DrawVLine(uint32_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Ypos + Length > ili9488_dev.height) return;
  for(counter = 0; counter < Length; counter++)
  {
    ili9488_WritePixel(Xpos, Ypos + counter, RGBCode);
  }
}


uint16_t ili9488_GetLcdPixelWidth(void)
{
  return ili9488_dev.width;
}

uint16_t ili9488_GetLcdPixelHeight(void)
{
  return ili9488_dev.height;
}


void ili9488_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
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
  ili9488_SetCursor(Xpos, Ypos);

  lcd_write_data((uint8_t*)pbmp, size*2);

  /* Set GRAM write direction and BGR = 0 */
  /* Memory access control: MY = 1, MX = 1, MV = 0, ML = 0 */
  write_cmd[1] = 0xC0;
  lcd_write_cmddata(write_cmd, sizeof(write_cmd));
}

void ili9488_Clear(uint32_t color)
{
	ili9488_SetCursor(0, 0);

	uint32_t totalpoint= ili9488_dev.width * ili9488_dev.height;
	uint8_t data[] = {color >> 16, color >> 8, color};
	for(uint32_t index = 0; index < totalpoint; index++)
	{
	  lcd_write_data(data, sizeof(data));
	}

	// for(uint32_t i = 0; i < sizeof(framebuff); i += 2)
	// {
	// 	framebuff[i] = color >> 8;
	// 	framebuff[i + 1] = color;
	// }

	// for(uint32_t index = 0; index < ili9488_dev.height/80; index++)
	// 	lcd_write_data(framebuff, sizeof(framebuff));
}

void ili9488_Init(void)
{
  lcd_panel_exec_cmd(ili9488_init_cmd, sizeof(ili9488_init_cmd));
  ili9488_SetDisplayWindow(0, 0, ili9488_dev.width, ili9488_dev.height);
  ili9488_DisplayOn();
  ili9488_Clear(WHITE);
}

LCD_DrvTypeDef   ili9488_drv =
{
  .Init = ili9488_Init,
  .ReadID = 0,
  .DisplayOn = ili9488_DisplayOn,
  .DisplayOff = ili9488_DisplayOff,
  .SetCursor = ili9488_SetCursor,
  .WritePixel = ili9488_WritePixel,
  .ReadPixel = 0,
  .SetDisplayWindow = ili9488_SetDisplayWindow,
  .DrawHLine = ili9488_DrawHLine,
  .DrawVLine = ili9488_DrawVLine,
  .GetLcdPixelWidth = ili9488_GetLcdPixelWidth,
  .GetLcdPixelHeight = ili9488_GetLcdPixelHeight,
  .DrawBitmap = ili9488_DrawBitmap,
};

LCD_DrvTypeDef* ili9488_probe(void)
{
	return &ili9488_drv;
}
