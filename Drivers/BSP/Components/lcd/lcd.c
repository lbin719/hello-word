#include "lcd.h"
#include "st7735s.h"
#include "ili9488.h"
#include "st7796.h"
#include "stm32f1xx_hal.h"
#include "ulog.h"
#include "main.h"
#include "spi.h"

LCD_DrvTypeDef *lcd_drv;

_lcd_dev lcd_dev;

uint32_t g_point_color = 0XF800;
uint32_t g_back_color  = 0XFFFFFF;

void lcd_write_data(uint8_t *data, uint16_t len)
{
	LCD_DC_HIGH();// data

 	LCD_CS_LOW();  //LCD_CS=0
//	if(len > 10)
//	{
//		spi3_dma_write(data, len);
//		spi3_dma_wait_finsh();
//	}
//	else
		spi3_bytes_write(data, len);
	LCD_CS_HIGH();  //LCD_CS=1
}

void lcd_write_cmddata(uint8_t *data, uint16_t len)
{
    LCD_DC_LOW();// cmd

	LCD_CS_LOW();  //LCD_CS=0

	spi3_bytes_write(&data[0], 1);

	if(len > 1)
	{
		LCD_DC_HIGH();
		spi3_bytes_write(&data[1], (len - 1));
	}

	LCD_CS_HIGH();  //LCD_CS=1
}

void lcd_read_cmddata(uint8_t cmd, uint8_t *data, uint16_t len)
{
    LCD_DC_LOW();

	LCD_CS_LOW();  //LCD_CS=0
	spi3_bytes_write(&cmd, 1);

	LCD_DC_HIGH();
	spi3_bytes_read(data, len);

	LCD_CS_HIGH();  //LCD_CS=1
}

void lcd_panel_exec_cmd(const uint8_t *cmd_table, uint32_t len)
{
    const uint8_t *cmd = cmd_table;
    uint32_t offset     = 0;

    if (!cmd_table || 0 == len)
        return ;

    while (offset < len)
    {
        if (CMD_TYPE_WR_CMD == cmd[CMD_IDX_TYPE])
            lcd_write_cmddata(&cmd[CMD_IDX_CODE], cmd[CMD_IDX_LEN]);
        else if (CMD_TYPE_DLY_MS == cmd[CMD_IDX_TYPE])
        	HAL_Delay(cmd[CMD_IDX_CODE]);

        offset += (cmd[CMD_IDX_LEN] + CMD_HEADER_LEN);
        cmd = cmd_table + offset;
    }
    return ;
}

void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    if(lcd_drv->WritePixel)
        lcd_drv->WritePixel(x, y, color);
}

void lcd_draw_hline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
    if(lcd_drv->DrawHLine)
        lcd_drv->DrawHLine(RGBCode, Xpos, Ypos, Length);
}

void lcd_draw_fill(uint16_t RGBCode, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    // if(lcd_drv->DrawFill)
    //     lcd_drv->DrawFill(RGBCode, sx, sy, ex, ey);
    uint16_t i, j;
    for(j = 0; j < (ey - sy + 1); j++)
    {
        for(i = 0; i < (ex - sx + 1); i++)
        {
            lcd_draw_point((sx + i), (sy + j), RGBCode);
        }
    }
}

void lcd_init(void)
{
#if LCD_DRIVER_IC_ST7735S
    lcd_drv = st7735s_probe();
#elif LCD_DRIVER_IC_ST7796
    lcd_drv = st7796_probe();
#elif LCD_DRIVER_IC_ILI9488
    lcd_drv = ili9488_probe();
#endif
    lcd_dev.width =  lcd_drv->GetLcdPixelWidth();
    lcd_dev.height = lcd_drv->GetLcdPixelHeight();

    GPIO_InitTypeDef gpio_init_struct = {0};

    LCD_RST_GPIO_CLK_ENABLE();
    LCD_DC_GPIO_CLK_ENABLE();
    LCD_CS_GPIO_CLK_ENABLE();
    LCD_BLK_GPIO_CLK_ENABLE();

    LCD_RST_HIGH();
    LCD_DC_HIGH();
    LCD_CS_HIGH();
    LCD_BLK_DISABLE();

    gpio_init_struct.Pin = LCD_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_RST_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_DC_GPIO_PIN;
    HAL_GPIO_Init(LCD_DC_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_BLK_GPIO_PIN;
    HAL_GPIO_Init(LCD_BLK_GPIO_PORT, &gpio_init_struct);

    spi3_init();

	LCD_RST_LOW();
	HAL_Delay(20);
    LCD_RST_HIGH();
	HAL_Delay(20);

    lcd_drv->Init();

    LCD_BLK_ENABLE();
}
