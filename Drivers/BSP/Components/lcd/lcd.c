#include "stm32f1xx_hal.h"
#include "ulog.h"
#include "main.h"
#include "spi.h"
#include "lcd.h"
#include "font.h" 

#define LCD_RST_HIGH()      (LCD_RST_GPIO_PORT->BSRR = LCD_RST_GPIO_PIN)
#define LCD_RST_LOW()       (LCD_RST_GPIO_PORT->BSRR = (uint32_t)(LCD_RST_GPIO_PIN  << 16u))

#define LCD_DC_HIGH()      (LCD_DC_GPIO_PORT->BSRR = LCD_DC_GPIO_PIN)
#define LCD_DC_LOW()       (LCD_DC_GPIO_PORT->BSRR = (uint32_t)(LCD_DC_GPIO_PIN  << 16u))

#define LCD_CS_HIGH()      (LCD_CS_GPIO_PORT->BSRR = LCD_CS_GPIO_PIN)
#define LCD_CS_LOW()       (LCD_CS_GPIO_PORT->BSRR = (uint32_t)(LCD_CS_GPIO_PIN  << 16u))

uint8_t framebuffer[LCD_WIDTH * 2];

//LCD的画笔颜色和背景色	   
uint16_t POINT_COLOR = 0x0000;	//画笔颜色
uint16_t BACK_COLOR = 0xFFFF;  //背景色 

//管理LCD重要参数
_lcd_dev lcddev;
	
//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(uint16_t regval)
{ 
	LCD_CS_LOW();  //LCD_CS=0
    LCD_DC_LOW();
    uint8_t data = regval&0x00FF;
	spi2_bytes_write(&data, 1);
	LCD_CS_HIGH();  //LCD_CS=1	   		 
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(uint16_t data)
{
    uint8_t w_d;

 	LCD_CS_LOW();  //LCD_CS=0
	LCD_DC_HIGH();
	// spi2_bytes_write((uint8_t *)&data, 2);
    w_d = data>>8;
	spi2_bytes_write(&w_d, 1);
    w_d = data;
	spi2_bytes_write(&w_d, 1);
	LCD_CS_HIGH();  //LCD_CS=1		
}
void LCD_WR_DATA8(uint8_t da)   //写8位数据
{
	LCD_CS_LOW();  //LCD_CS=0
	LCD_DC_HIGH();			    	   
	spi2_bytes_write(&da, 1);	
	LCD_CS_HIGH();  //LCD_CS=1   			 
}					   
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WR_REG_DATA(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}
//开始写GRAM
void lcd_write_gram(void)
{
	LCD_WR_REG(0x2c);  
}	 
	 

void lcd_write_data(const uint8_t *data, uint16_t len)
{
 	LCD_CS_LOW();  //LCD_CS=0
	LCD_DC_HIGH();
	spi2_bytes_write((uint8_t *)data, len);
	LCD_CS_HIGH();  //LCD_CS=1		
}

void lcd_write_cmd(const uint8_t *cmd, uint32_t len)
{
	LCD_WR_REG(cmd[0]);
	if(len > 1)
		lcd_write_data(&cmd[1], (len - 1));
}	 
//LCD开启显示
void LCD_DisplayOn(void)
{					   

}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   

}   

  

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	lcd_setcursor(x,y);		//设置光标位置 
	lcd_write_gram();	//开始写入GRAM
	LCD_WR_DATA(POINT_COLOR); 
} 
  
//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=LCD_WIDTH;
	totalpoint *= LCD_HEIGHT; 	//得到总点数
	lcd_setcursor(0, 0);	//设置光标位置
	lcd_write_gram();     //开始写入GRAM	 	  
	for(index = 0; index < totalpoint; index++)
	{
		LCD_WR_DATA(color);
	}

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
	//  	lcd_setcursor(sx,i);      				//设置光标位置 
	// 	lcd_write_gram();     			//开始写入GRAM	  
	// 	for(j=0;j<xlen;j++)LCD_WR_DATA(color);	//设置光标位置 	    
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
 		lcd_setcursor(sx,sy+i);   	//设置光标位置 
		lcd_write_gram();     //开始写入GRAM
		for(j=0;j<width;j++){
					// LCD->LCD_RAM=color[i*height+j];//写入数据 
			LCD_WR_DATA(*color);
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
//	unsigned char i,j,k;
//	const unsigned char *temp=hanzi16;
//	temp+=index*32;
//	for(j=0;j<16;j++)
//	{
//		lcd_setcursor(x,y+j);
//		lcd_write_gram();	//开始写入GRAM
//		for(k=0;k<2;k++)
//		{
//			for(i=0;i<8;i++)
//			{
//			 	if((*temp&(1<<i))!=0)
//				{
//					LCD_WR_DATA(POINT_COLOR);
//				}
//				else
//				{
//					LCD_WR_DATA(BACK_COLOR);
//				}
//			}
//			temp++;
//		}
//	 }
}	
//在指定位置显示一个汉字(32*32大小)
void showhanzi32(unsigned int x,unsigned int y,unsigned char index)	
{  
	unsigned char i,j,k;
	const unsigned char *temp=hanzi32;
	temp+=index*128;
	for(j=0;j<32;j++)
	{
		lcd_setcursor(x,y+j);
		lcd_write_gram();	//开始写入GRAM
		for(k=0;k<4;k++)
		{
			for(i=0;i<8;i++)
			{
			 	if((*temp&(1<<i))!=0)
				{
					LCD_WR_DATA(POINT_COLOR);
				}
				else
				{
					LCD_WR_DATA(BACK_COLOR);
				}
			}
			temp++;
		}
	 }
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
				if(y>=LCD_HEIGHT){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=LCD_WIDTH){POINT_COLOR=colortemp;return;}//超区域了
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
				if(y>=LCD_HEIGHT){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=LCD_WIDTH){POINT_COLOR=colortemp;return;}//超区域了
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
        LCD_ShowChar(x,y,*p,size,1);
        x+=size/2;
        p++;
    }  
}

void showimage(uint16_t x,uint16_t y) //显示40*40图片
{  
	// uint16_t i,j,k;
	// uint16_t da;
	// k=0;
	// for(i=0;i<40;i++)
	// {
	// 	lcd_setcursor(x,y+i);
	// 	lcd_write_gram();     			//开始写入GRAM
	// 	for(j=0;j<40;j++)
	// 	{
	// 		da=qqimage[k*2+1];
	// 		da<<=8;
	// 		da|=qqimage[k*2];
	// 		LCD_WR_DATA(da);
	// 		k++;
	// 	}
	// }
}


void lcd_reset(void)
{
	LCD_RST_LOW();	//LCD_RST=0	 //SPI接口复位
	HAL_Delay(20);   // delay 20 ms 
    LCD_RST_HIGH();	//LCD_RST=1		
	HAL_Delay(20);
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
            lcd_write_cmd(&cmd[CMD_IDX_CODE], cmd[CMD_IDX_LEN]);
        else if (CMD_TYPE_DLY_MS == cmd[CMD_IDX_TYPE])
        	HAL_Delay(cmd[CMD_IDX_CODE]);

        offset += (cmd[CMD_IDX_LEN] + CMD_HEADER_LEN);
        cmd = cmd_table + offset;
    }
    return ;
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
	DISP_WR_CMD(0x36, 0xC0), // MX, MY, RGB mode
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	DISP_WR_CMD(0xE0, 0x04, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13),
	DISP_WR_CMD(0xE1, 0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13),
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	DISP_WR_CMD(0x2A, 0x00, 0x00, 0x00, 0x7F),
	DISP_WR_CMD(0x2B, 0x00, 0x00, 0x00, 0x9F),
	DISP_WR_CMD(0x3A, 0x05), //65k mode
	DISP_WR_CMD(0x29), //Display on
};

//设置光标位置
//xpos:横坐标
//ypos:纵坐标
void lcd_setcursor(uint16_t xpos, uint16_t ypos)
{
	uint8_t setcursor_xpos_cmd[] = {0x2A, (uint8_t)(xpos>>8), (uint8_t)(xpos&0XFF), (uint8_t)((LCD_WIDTH - 1)>>8), (uint8_t)((LCD_WIDTH - 1)&0XFF)};
	uint8_t setcursor_ypos_cmd[] = {0x2B, (uint8_t)(ypos>>8), (uint8_t)(ypos&0XFF), (uint8_t)((LCD_HEIGHT - 1)>>8), (uint8_t)((LCD_HEIGHT - 1)&0XFF)};

	lcd_write_cmd((const uint8_t *)setcursor_xpos_cmd, sizeof(setcursor_xpos_cmd));
	lcd_write_cmd((const uint8_t *)setcursor_ypos_cmd, sizeof(setcursor_ypos_cmd));
} 	

void xianshi()//ÏÔÊ¾ÐÅÏ¢
{ 
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;   
	//ÏÔÊ¾32*32ºº×Ö
	// showhanzi32(0,0,0);	 //ÌÔ
	// showhanzi32(40,0,1);	 //¾§
	// showhanzi32(80,0,2);    //³Û
	// //ÏÔÊ¾16*16ºº×Ö
	// showhanzi16(0,35,0);	  //×¨
	// showhanzi16(20,35,1);	  //×¢
	// showhanzi16(40,35,2);	  //ÏÔ
	// showhanzi16(60,35,3);	  //Ê¾
	// showhanzi16(80,35,4);	  //·½
	// showhanzi16(100,35,5);	  //°¸	   
//	LCD_ShowString(0,55,200,16,16,"1.8 TFT SPI");
}

void lcd_init(void)
{
	// LCD_WIDTH=128;
	// LCD_HEIGHT=160;
	// lcddev.wramcmd=0X2C;
	// lcddev.setxcmd=0X2A;
	// lcddev.setycmd=0X2B; 	

    GPIO_InitTypeDef gpio_init_struct = {0};

    LCD_RST_GPIO_CLK_ENABLE();
    LCD_DC_GPIO_CLK_ENABLE();
    LCD_CS_GPIO_CLK_ENABLE();      /* CS脚 时钟使能 */

    gpio_init_struct.Pin = LCD_RST_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_RST_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_DC_GPIO_PIN;
    HAL_GPIO_Init(LCD_DC_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = LCD_CS_GPIO_PIN;
    HAL_GPIO_Init(LCD_CS_GPIO_PORT, &gpio_init_struct);

    LCD_RST_HIGH();
    LCD_DC_HIGH();
    LCD_CS_HIGH();

    spi2_init();

    lcd_reset();
    // lcd_init_config();
	lcd_panel_exec_cmd(st7735s_128x160_init_cmd, sizeof(st7735s_128x160_init_cmd));

//	lcd_setcursor(0x00, 0x50);
	LCD_Clear(RED);
 	POINT_COLOR=GREEN;

	LCD_Color_Fill(40, 90, 100, 100, &POINT_COLOR);
	// xianshi();	   //显示信息
	// showqq();	   //显示QQ

	while(1) {
		// LCD_Clear(RED); 
		// LCD_Clear(GREEN); 
		// LCD_Clear(BLUE); 
	}
}
