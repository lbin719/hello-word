#include "stm32f1xx_hal.h"
#include "ulog.h"
#include "main.h"
#include "spi.h"
#include "st7735s.h"
#include "lcd.h"
#include "font.h"

#define LCD_RST_HIGH()      (LCD_RST_GPIO_PORT->BSRR = LCD_RST_GPIO_PIN)
#define LCD_RST_LOW()       (LCD_RST_GPIO_PORT->BSRR = (uint32_t)(LCD_RST_GPIO_PIN  << 16u))

#define LCD_DC_HIGH()      (LCD_DC_GPIO_PORT->BSRR = LCD_DC_GPIO_PIN)
#define LCD_DC_LOW()       (LCD_DC_GPIO_PORT->BSRR = (uint32_t)(LCD_DC_GPIO_PIN  << 16u))

#define LCD_CS_HIGH()      (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN)
#define LCD_CS_LOW()       (LCD_CS_GPIO_PORT->BSRR = (uint32_t)(LCD_CS_GPIO_PIN  << 16u))


// static uint8_t framebuff[ST7735S_LCD_PIXEL_WIDTH * 2 * 80];

//LCD的画笔颜色和背景色
uint16_t POINT_COLOR = 0x0000;	//画笔颜色
uint16_t BACK_COLOR = 0xFFFF;  //背景色

_lcd_dev lcddev;



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
	spi3_bytes_write(cmd, 1);

	LCD_DC_HIGH();
	spi3_bytes_read(data, len);

	LCD_CS_HIGH();  //LCD_CS=1
}

//写LCD数据
//data:要写入的值
void lcd_wr_data(uint16_t RGBCode)
{
	uint8_t data[] = {RGBCode >> 8, RGBCode};
	lcd_write_data(data, sizeof(data));
}


//LCD开启显示
void st7735s_DisplayOn(void)
{
//   uint8_t data = 0;
//   LCD_IO_WriteReg(LCD_REG_19);
//   LCD_Delay(10);
//   LCD_IO_WriteReg(LCD_REG_41);
//   LCD_Delay(10);
//   LCD_IO_WriteReg(LCD_REG_54);
//   data = 0xC0;
//   LCD_IO_WriteMultipleData(&data, 1);
}
//LCD关闭显示
void st7735s_DisplayOff(void)
{

}


//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	st7735s_SetCursor(x,y);		//设置光标位置
	lcd_wr_data(POINT_COLOR);
}

//清屏函数
//color:要清屏的填充色
void lcd_clear(uint16_t color)
{
	st7735s_SetCursor(0, 0);

	uint32_t totalpoint=ST7735S_LCD_PIXEL_WIDTH;
	totalpoint *= ST7735S_LCD_PIXEL_HEIGHT; 	//得到总点数
	for(uint32_t index = 0; index < totalpoint; index++)
	{
		lcd_wr_data(color);
	}

	// for(uint32_t i = 0; i < sizeof(framebuff); i += 2)
	// {
	// 	framebuff[i] = color >> 8;
	// 	framebuff[i + 1] = color;
	// }

	// for(uint32_t index = 0; index < ST7735S_LCD_PIXEL_HEIGHT/80; index++)
	// 	lcd_write_data(framebuff, sizeof(framebuff));


}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
	// uint16_t i,j;
	// uint16_t xlen=0;
	// xlen=ex-sx+1;
	// for(i=sy;i<=ey;i++)
	// {
	//  	st7735s_SetCursor(sx,i);      				//设置光标位置
	// 	for(j=0;j<xlen;j++)lcd_wr_data(color);	//设置光标位置
	// }
}
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 		//得到填充的宽度
	height=ey-sy+1;		//高度
 	for(i=0;i<height;i++)
	{
 		st7735s_SetCursor(sx,sy+i);   	//设置光标位置
		for(j=0;j<width;j++){
					// LCD->LCD_RAM=color[i*height+j];//写入数据
			lcd_wr_data(*color);
		}
	}
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// uint16_t t;
	// int xerr=0,yerr=0,delta_x,delta_y,distance;
	// int incx,incy,uRow,uCol;
	// delta_x=x2-x1; //计算坐标增量
	// delta_y=y2-y1;
	// uRow=x1;
	// uCol=y1;
	// if(delta_x>0)incx=1; //设置单步方向
	// else if(delta_x==0)incx=0;//垂直线
	// else {incx=-1;delta_x=-delta_x;}
	// if(delta_y>0)incy=1;
	// else if(delta_y==0)incy=0;//水平线
	// else{incy=-1;delta_y=-delta_y;}
	// if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
	// else distance=delta_y;
	// for(t=0;t<=distance+1;t++ )//画线输出
	// {
	// 	LCD_DrawPoint(uRow,uCol);//画点
	// 	xerr+=delta_x ;
	// 	yerr+=delta_y ;
	// 	if(xerr>distance)
	// 	{
	// 		xerr-=distance;
	// 		uRow+=incx;
	// 	}
	// 	if(yerr>distance)
	// 	{
	// 		yerr-=distance;
	// 		uCol+=incy;
	// 	}
	// }
}
//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	// LCD_DrawLine(x1,y1,x2,y1);
	// LCD_DrawLine(x1,y1,x1,y2);
	// LCD_DrawLine(x1,y2,x2,y2);
	// LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	// int a,b;
	// int di;
	// a=0;b=r;
	// di=3-(r<<1);             //判断下个点位置的标志
	// while(a<=b)
	// {
	// 	LCD_DrawPoint(x0+a,y0-b);             //5
 	// 	LCD_DrawPoint(x0+b,y0-a);             //0
	// 	LCD_DrawPoint(x0+b,y0+a);             //4
	// 	LCD_DrawPoint(x0+a,y0+b);             //6
	// 	LCD_DrawPoint(x0-a,y0+b);             //1
 	// 	LCD_DrawPoint(x0-b,y0+a);
	// 	LCD_DrawPoint(x0-a,y0-b);             //2
  	// 	LCD_DrawPoint(x0-b,y0-a);             //7
	// 	a++;
	// 	//使用Bresenham算法画圆
	// 	if(di<0)di +=4*a+6;
	// 	else
	// 	{
	// 		di+=10+4*(a-b);
	// 		b--;
	// 	}
	// }
}
//在指定位置显示一个汉字(16*16大小)
void showhanzi16(unsigned int x,unsigned int y,unsigned char index)
{
	unsigned char i,j,k;
	const unsigned char *temp=hanzi16;
	temp+=index*32;
	for(j=0;j<16;j++)
	{
		st7735s_SetCursor(x,y+j);
		for(k=0;k<2;k++)
		{
			for(i=0;i<8;i++)
			{
			 	if((*temp&(1<<i))!=0)
				{
					lcd_wr_data(POINT_COLOR);
				}
				else
				{
					lcd_wr_data(BACK_COLOR);
				}
			}
			temp++;
		}
	 }
}
//在指定位置显示一个汉字(32*32大小)
void showhanzi32(unsigned int x,unsigned int y,unsigned char index)
{
	// unsigned char i,j,k;
	// const unsigned char *temp=hanzi32;
	// temp+=index*128;
	// for(j=0;j<32;j++)
	// {
	// 	st7735s_SetCursor(x,y+j);
	// 	for(k=0;k<4;k++)
	// 	{
	// 		for(i=0;i<8;i++)
	// 		{
	// 		 	if((*temp&(1<<i))!=0)
	// 			{
	// 				lcd_wr_data(POINT_COLOR);
	// 			}
	// 			else
	// 			{
	// 				lcd_wr_data(BACK_COLOR);
	// 			}
	// 		}
	// 		temp++;
	// 	}
	//  }
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
  uint8_t temp,t1,t;
	uint16_t y0=y;
	uint16_t colortemp=POINT_COLOR;
	//设置窗口
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
	    for(t=0;t<size;t++)
	    {
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体
	        for(t1=0;t1<8;t1++)
			{
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);
				temp<<=1;
				y++;
				if(y>=ST7735S_LCD_PIXEL_HEIGHT){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=ST7735S_LCD_PIXEL_WIDTH){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}
	    }
	}else//叠加方式
	{
	    for(t=0;t<size;t++)
	    {
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体
	        for(t1=0;t1<8;t1++)
			{
		        if(temp&0x80)LCD_DrawPoint(x,y);
				temp<<=1;
				y++;
				if(y>=ST7735S_LCD_PIXEL_HEIGHT){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=ST7735S_LCD_PIXEL_WIDTH){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}
	    }
	}
	POINT_COLOR=colortemp;
}
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{
	// uint8_t t,temp;
	// uint8_t enshow=0;
	// for(t=0;t<len;t++)
	// {
	// 	temp=(num/LCD_Pow(10,len-t-1))%10;
	// 	if(enshow==0&&t<(len-1))
	// 	{
	// 		if(temp==0)
	// 		{
	// 			LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
	// 			continue;
	// 		}else enshow=1;

	// 	}
	//  	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
	// }
}
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{
	// uint8_t t,temp;
	// uint8_t enshow=0;
	// for(t=0;t<len;t++)
	// {
	// 	temp=(num/LCD_Pow(10,len-t-1))%10;
	// 	if(enshow==0&&t<(len-1))
	// 	{
	// 		if(temp==0)
	// 		{
	// 			if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);
	// 			else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);
 	// 			continue;
	// 		}else enshow=1;

	// 	}
	//  	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01);
	// }
}
//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}

void showimage(uint16_t x,uint16_t y) //显示40*40图片
{
//	 uint16_t i,j,k;
//	 uint16_t da;
//	 k=0;
//	 for(i=0;i<40;i++)
//	 {
//	 	st7735s_SetCursor(x,y+i);
//	 	for(j=0;j<40;j++)
//	 	{
//	 		da=qqimage[k*2+1];
//	 		da<<=8;
//	 		da|=qqimage[k*2];
//	 		lcd_wr_data(da);
//	 		k++;
//	 	}
//	 }
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
#if USE_HORIZONTAL == 0
	// DISP_WR_CMD(0x36, 0xC0), // MX, MY, RGB mode 1100
	DISP_WR_CMD(0x36, 0x40), // MX, MY, RGB mode 0100
#elif USE_HORIZONTAL == 1
	DISP_WR_CMD(0x36, 0x80), // MX, MY, RGB mode 1000
#elif USE_HORIZONTAL == 2
	// DISP_WR_CMD(0x36, 0xE0), // MX, MY, RGB mode 1110
	// DISP_WR_CMD(0x36, 0xA0), // MX, MY, RGB mode 1010
	// DISP_WR_CMD(0x36, 0x20), // MX, MY, RGB mode 0010
	DISP_WR_CMD(0x36, 0x60), // MX, MY, RGB mode 0110
#elif USE_HORIZONTAL == 3
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
  uint8_t data = 0;
  if((Xpos >= ST7735S_LCD_PIXEL_WIDTH) || (Ypos >= ST7735S_LCD_PIXEL_HEIGHT))
  {
    return;
  }

  /* Set Cursor */
  st7735s_SetCursor(Xpos, Ypos);

  lcd_wr_data(RGBCode);
}

void st7735s_Write_Gram(uint16_t RGBCode)
{
  lcd_wr_data(RGBCode);
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
  uint8_t counter = 0;

  if(Xpos + Length > ST7735S_LCD_PIXEL_WIDTH) return;

  /* Set Cursor */
  st7735s_SetCursor(Xpos, Ypos);

  for(counter = 0; counter < Length; counter++)
  {
//    ArrayRGB[counter] = RGBCode;
	  lcd_wr_data(RGBCode);
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
  uint8_t counter = 0;

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
  st7735_SetCursor(Xpos, Ypos);

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

void st7735s_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    LCD_RST_GPIO_CLK_ENABLE();
    LCD_DC_GPIO_CLK_ENABLE();
    LCD_CS_GPIO_CLK_ENABLE();      /* CS脚 时钟使能 */

    gpio_init_struct.Pin = LCD_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_RST_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_DC_GPIO_PIN;
    HAL_GPIO_Init(LCD_DC_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);

    LCD_RST_HIGH();
    LCD_DC_HIGH();
    LCD_CS_HIGH();

    spi3_init();

	LCD_RST_LOW();	//LCD_RST=0	 //SPI接口复位
	HAL_Delay(20);   // delay 20 ms
    LCD_RST_HIGH();	//LCD_RST=1
	HAL_Delay(20);
	lcd_panel_exec_cmd(st7735s_128x160_init_cmd, sizeof(st7735s_128x160_init_cmd));
	st7735s_SetDisplayWindow(0, 0, ST7735S_LCD_PIXEL_WIDTH, ST7735S_LCD_PIXEL_HEIGHT);
	lcd_clear(WHITE);
	// lcd_clear(RED);
	// HAL_Delay(2000);
 	POINT_COLOR=RED;
	// LCD_Color_Fill(10, 10, 20, 60, &POINT_COLOR);
	showhanzi16(0,35,6);	  //
	showhanzi16(20,35,7);	  //
	showhanzi16(40,35,8);	  //
	showhanzi16(60,35,9);	  //
	showhanzi16(80,35,10);	  //
	showhanzi16(100,35,11);	  //


	uint8_t test_buf[32];
//	uint8_t t_d[3];
	while(1)
	{
//		lcd_read_cmddata(0x0c, t_d, 1);
//		LOG_I("READ:%02x \r\n", t_d[0]);
		//  lcd_clear(WHITE);
		//  HAL_Delay(200);
		//  lcd_clear(RED);
		//  HAL_Delay(200);
		//  lcd_clear(BLUE);
		//  HAL_Delay(200);
		//  lcd_clear(GREEN);
		//  HAL_Delay(200);
		//  lcd_clear(BLACK);
		snprintf(test_buf, sizeof(test_buf), "Tick:%ld", HAL_GetTick());
		LCD_ShowString(0, 55, 200, 16, 16, test_buf);
		// HAL_Delay(200);
//   uint8_t write_cmd[] = {0x36, 0x40};
//   lcd_write_cmddata(write_cmd, sizeof(write_cmd));
//   HAL_Delay(200);
	}




// 	BACK_COLOR=WHITE;
// 	POINT_COLOR=RED;

// 	// showhanzi32(0,0,0);	 //
// 	// showhanzi32(40,0,1);	 //
// 	// showhanzi32(80,0,2);    //

// 	showhanzi16(0,35,6);	  //
// 	showhanzi16(20,35,7);	  //
// 	showhanzi16(40,35,8);	  //
// 	showhanzi16(60,35,9);	  //
// 	showhanzi16(80,35,10);	  //
// 	showhanzi16(100,35,11);	  //
// 	LCD_ShowString(0,55,200,16,16,"Yifeichongtian");

// 	// showqq();	   //显示QQ

// 	while(1) {
// 		// lcd_clear(RED);
// 		// lcd_clear(GREEN);
// 		// lcd_clear(BLUE);
// 		LCD_ShowString(0,75,200,16,16,"test");
// 	}
}

// LCD_DrvTypeDef   st7735s_drv =
// {
//   st7735s_Init,
//   0,
//   st7735s_DisplayOn,
//   st7735s_DisplayOff,
//   st7735s_SetCursor,
//   st7735s_WritePixel,
//   0,
//   st7735s_SetDisplayWindow,
//   st7735s_DrawHLine,
//   st7735s_DrawVLine,
//   st7735s_GetLcdPixelWidth,
//   st7735s_GetLcdPixelHeight,
//   st7735s_DrawBitmap,
// };
