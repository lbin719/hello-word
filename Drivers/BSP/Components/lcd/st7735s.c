#include "st7735s.h"
#include "lcd.h"

#if LCD_HORIZONTAL==0 || LCD_HORIZONTAL==1
#define  ST7735S_LCD_PIXEL_WIDTH    ((uint16_t)128)
#define  ST7735S_LCD_PIXEL_HEIGHT   ((uint16_t)160)
#else
#define  ST7735S_LCD_PIXEL_WIDTH    ((uint16_t)160)
#define  ST7735S_LCD_PIXEL_HEIGHT   ((uint16_t)128)
#endif

#define LCD_RST_HIGH()      (LCD_RST_GPIO_PORT->BSRR = LCD_RST_GPIO_PIN)
#define LCD_RST_LOW()       (LCD_RST_GPIO_PORT->BSRR = (uint32_t)(LCD_RST_GPIO_PIN  << 16u))

#define LCD_DC_HIGH()      (LCD_DC_GPIO_PORT->BSRR = LCD_DC_GPIO_PIN)
#define LCD_DC_LOW()       (LCD_DC_GPIO_PORT->BSRR = (uint32_t)(LCD_DC_GPIO_PIN  << 16u))

#define LCD_CS_HIGH()      (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN)
#define LCD_CS_LOW()       (LCD_CS_GPIO_PORT->BSRR = (uint32_t)(LCD_CS_GPIO_PIN  << 16u))

// static uint8_t framebuff[ST7735S_LCD_PIXEL_WIDTH * 2 * 80];

_lcd_dev st7735s_dev = {
	.width = ST7735S_LCD_PIXEL_WIDTH,
	.height = ST7735S_LCD_PIXEL_HEIGHT,
    .id = 0x0000,
    .wramcmd = 0x2C,
    .setxcmd = 0x2A,
    .setycmd = 0x2B
};

void st7735s_DisplayOn(void)
{

}

void st7735s_DisplayOff(void)
{

}

static const uint8_t st7735s_128x160_init_cmd[] =
{
	DISP_WR_CMD(0x11), //Sleep out
	DISP_DLY_MS(120), //Delay 120ms
	//------------------------------------ST7735S Frame Rate-----------------------------------------//
	DISP_WR_CMD(0xB1, 0x05, 0x3C, 0x3C),
	DISP_WR_CMD(0xB2, 0x05, 0x3C, 0x3C),
	DISP_WR_CMD(0xB3, 0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C),
	//------------------------------------End ST7735S Frame Rate-----------------------------------------//
	DISP_WR_CMD(0xB4, 0x03), //Dot inversio
	DISP_WR_CMD(0xC0, 0x28, 0x08, 0x04),
	DISP_WR_CMD(0xC1, 0XC0),
	DISP_WR_CMD(0xC2, 0x0D, 0x00),
	DISP_WR_CMD(0xC3, 0x8D, 0x2A),
	DISP_WR_CMD(0xC4, 0x8D, 0xEE),
	//---------------------------------End ST7735S Power Sequence-----------------------------)--------//
	DISP_WR_CMD(0xC5, 0x1A), //VCO
#if LCD_HORIZONTAL == 0
	// DISP_WR_CMD(0x36, 0xC0), // MX, MY, RGB mode 1100
	DISP_WR_CMD(0x36, 0x40), // MX, MY, RGB mode 0100
#elif LCD_HORIZONTAL == 1
	DISP_WR_CMD(0x36, 0x80), // MX, MY, RGB mode 1000
#elif LCD_HORIZONTAL == 2
	// DISP_WR_CMD(0x36, 0xE0), // MX, MY, RGB mode 1110
	// DISP_WR_CMD(0x36, 0xA0), // MX, MY, RGB mode 1010
	// DISP_WR_CMD(0x36, 0x20), // MX, MY, RGB mode 0010
	DISP_WR_CMD(0x36, 0x60), // MX, MY, RGB mode 0110
#elif LCD_HORIZONTAL == 3
	DISP_WR_CMD(0x36, 0x20), // MX, MY, RGB mode 0010
#endif

	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	DISP_WR_CMD(0xE0, 0x04, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13),
	DISP_WR_CMD(0xE1, 0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13),
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	// DISP_WR_CMD(0x2A, 0x00, 0x00, 0x00, 0x7F),
	// DISP_WR_CMD(0x2B, 0x00, 0x00, 0x00, 0x9F),
	DISP_WR_CMD(0x3A, 0x05), //65k mode
	DISP_WR_CMD(0x29), //Display on
};

/**
  * @brief  Sets Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void st7735s_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF)};

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
void st7735s_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
	if((Xpos >= ST7735S_LCD_PIXEL_WIDTH) || (Ypos >= ST7735S_LCD_PIXEL_HEIGHT))
	{
		return;
	}

	/* Set Cursor */
	st7735s_SetCursor(Xpos, Ypos);

	uint8_t data[] = {RGBCode >> 8, RGBCode};
	lcd_write_data(data, sizeof(data));
}

/**
  * @brief  Sets a display window
  * @param  Xpos:   specifies the X bottom left position.
  * @param  Ypos:   specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width:  display window width.
  * @retval None
  */
void st7735s_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(Xpos>>8), (uint8_t)(Xpos&0XFF), (uint8_t)((Xpos + Width - 1)>>8), (uint8_t)((Xpos + Width - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(Ypos>>8), (uint8_t)(Ypos&0XFF), (uint8_t)((Ypos + Height - 1)>>8), (uint8_t)((Ypos + Height - 1)&0XFF)};

	lcd_write_cmddata(setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmddata(setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));
}

/**
  * @brief  Draws horizontal line.
  * @param  RGBCode: Specifies the RGB color
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Length: specifies the line length.
  * @retval None
  */
void st7735s_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Xpos + Length > ST7735S_LCD_PIXEL_WIDTH) return;

  /* Set Cursor */
  st7735s_SetCursor(Xpos, Ypos);

	uint8_t data[] = {RGBCode >> 8, RGBCode};
  for(counter = 0; counter < Length; counter++)
  {
//    ArrayRGB[counter] = RGBCode;
	lcd_write_data(data, sizeof(data));
  }

//  lcd_write_data((uint8_t*)&ArrayRGB[0], Length * 2);
}

/**
  * @brief  Draws vertical line.
  * @param  RGBCode: Specifies the RGB color
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Length: specifies the line length.
  * @retval None
  */
void st7735s_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter = 0;

  if(Ypos + Length > ST7735S_LCD_PIXEL_HEIGHT) return;
  for(counter = 0; counter < Length; counter++)
  {
    st7735s_WritePixel(Xpos, Ypos + counter, RGBCode);
  }
}

/**
  * @brief  Gets the LCD pixel Width.
  * @param  None
  * @retval The Lcd Pixel Width
  */
uint16_t st7735s_GetLcdPixelWidth(void)
{
  return ST7735S_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Gets the LCD pixel Height.
  * @param  None
  * @retval The Lcd Pixel Height
  */
uint16_t st7735s_GetLcdPixelHeight(void)
{
  return ST7735S_LCD_PIXEL_HEIGHT;
}

/**
  * @brief  Displays a bitmap picture loaded in the internal Flash.
  * @param  BmpAddress: Bmp picture address in the internal Flash.
  * @retval None
  */
void st7735s_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
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
  st7735s_SetCursor(Xpos, Ypos);

  lcd_write_data((uint8_t*)pbmp, size*2);

  /* Set GRAM write direction and BGR = 0 */
  /* Memory access control: MY = 1, MX = 1, MV = 0, ML = 0 */
  write_cmd[1] = 0xC0;
  lcd_write_cmddata(write_cmd, sizeof(write_cmd));
}

// void showqq()
// {
// 	uint16_t x,y;
// 	x=0;
// 	y=75;
// 	while(y < ST7735S_LCD_PIXEL_HEIGHT - 39)
// 	{
// 		x=0;
// 		while(x < ST7735S_LCD_PIXEL_WIDTH - 39)
// 		{
// 			showimage(x, y);
// 			x+=40;
// 		}
// 		y+=40;
// 	 }
// }
//娓呭睆鍑芥暟
//color:瑕佹竻灞忕殑濉?鍏呰壊
void lcd_clear(uint16_t color)
{
	st7735s_SetCursor(0, 0);

	uint32_t totalpoint=ST7735S_LCD_PIXEL_WIDTH * ST7735S_LCD_PIXEL_HEIGHT;
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

	// for(uint32_t index = 0; index < ST7735S_LCD_PIXEL_HEIGHT/80; index++)
	// 	lcd_write_data(framebuff, sizeof(framebuff));


}


void st7735s_Init(void)
{
	lcd_panel_exec_cmd(st7735s_128x160_init_cmd, sizeof(st7735s_128x160_init_cmd));
	st7735s_SetDisplayWindow(0, 0, ST7735S_LCD_PIXEL_WIDTH, ST7735S_LCD_PIXEL_HEIGHT);
	lcd_clear(WHITE);

}

void st7735s_test(void)
{

// 	// // LCD_Color_Fill(10, 10, 20, 60, &POINT_COLOR);

// 	uint8_t test_buf[32];
// //	uint8_t t_d[3];
// 	while(1)
// 	{
// //		lcd_read_cmddata(0x0c, t_d, 1);
// //		LOG_I("READ:%02x \r\n", t_d[0]);
// 		//  lcd_clear(WHITE);
// 		//  HAL_Delay(200);
// 		//  lcd_clear(RED);
// 		//  HAL_Delay(200);
// 		//  lcd_clear(BLUE);
// 		//  HAL_Delay(200);
// 		//  lcd_clear(GREEN);
// 		//  HAL_Delay(200);
// 		//  lcd_clear(BLACK);
// 		// snprintf(test_buf, sizeof(test_buf), "Tick:%ld", HAL_GetTick());
// 		// LCD_ShowString(0, 55, 200, 16, 16, test_buf);
// 		// HAL_Delay(200);
// //   uint8_t write_cmd[] = {0x36, 0x40};
// //   lcd_write_cmddata(write_cmd, sizeof(write_cmd));
// //   HAL_Delay(200);
// 	// }

}


LCD_DrvTypeDef   st7735s_drv =
{
  st7735s_Init,
  0,
  st7735s_DisplayOn,
  st7735s_DisplayOff,
  st7735s_SetCursor,
  st7735s_WritePixel,
  0,
  st7735s_SetDisplayWindow,
  st7735s_DrawHLine,
  st7735s_DrawVLine,
  st7735s_GetLcdPixelWidth,
  st7735s_GetLcdPixelHeight,
  st7735s_DrawBitmap,
};

LCD_DrvTypeDef* st7735s_probe(void)
{
	return &st7735s_drv;
}
