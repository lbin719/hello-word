#include "stm32f1xx_hal.h"
#include "delay.h"
#include "hx711.h"
#include "board.h"
#include "ulog.h"

#define HX711_SCK_HIGH()        	(HX711_SCK_GPIO_PORT->BSRR = HX711_SCK_GPIO_PIN)
#define HX711_SCK_LOW()       		(HX711_SCK_GPIO_PORT->BSRR = (uint32_t)(HX711_SCK_GPIO_PIN  << 16u))

#define HX711_DOUT_HIGH()        	(HX711_DOUT_GPIO_PORT->BSRR = HX711_DOUT_GPIO_PIN)
// #define HX711_DOUT_LOW()      		(HX711_DOUT_GPIO_PORT->BSRR = (uint32_t)(HX711_DOUT_GPIO_PIN  << 16u))
#define HX711_DOUT_PIN()			HAL_GPIO_ReadPin(HX711_DOUT_GPIO_PORT, HX711_DOUT_GPIO_PIN)

//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
#define GapValue 109.578//106.5


uint32_t Weight_Maopi;
int32_t Weight_Shiwu;


//****************************************************
//读取HX711
//****************************************************
uint32_t hx711_read(void)	//增益128
{
	uint32_t wait_timeout = 0;
	unsigned long count;
	unsigned char i;

  	// HX711_DOUT_HIGH();
	// delay_us(1);
  	HX711_SCK_LOW();//使能AD（PD_SCK 置低）

  	while(HX711_DOUT_PIN())//AD转换未结束则等待，否则开始读取
	{
		if(wait_timeout++ > 10)
		{
			return 0;
		}
		delay_us(10);
	}

  	count = 0;
  	for(i = 0; i < 24; i++)
	{
	  	HX711_SCK_HIGH();
	  	count = count << 1;
		delay_us(1);
		HX711_SCK_LOW();
	  	if(HX711_DOUT_PIN())
			count++;
		delay_us(1);
	}

 	HX711_SCK_HIGH();
    count = count ^ 0x800000;//第25个脉冲下降沿来时，转换数据
	delay_us(1);
	HX711_SCK_LOW();
	return(count);
}

void hx711_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    HX711_SCK_GPIO_CLK_ENABLE();
	HX711_DOUT_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = HX711_SCK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(HX711_SCK_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = HX711_DOUT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(HX711_DOUT_GPIO_PORT, &gpio_init_struct);

	Weight_Maopi = hx711_read();
	LOG_I("HX711 Maopi: %d\r\n", Weight_Maopi);
}

//****************************************************
//获取毛皮重量
//****************************************************
void hx711_get_maopi(void)
{
	Weight_Maopi = hx711_read();
}

//****************************************************
//称重
//****************************************************
void hx711_get_weight(void)
{
	uint32_t read_value = hx711_read();
	if(read_value > Weight_Maopi)
	{
		Weight_Shiwu = read_value - Weight_Maopi;				//获取实物的AD采样数值。

		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 		//计算实物的实际重量
																		//因为不同的传感器特性曲线不一样，因此，每一个传感器需要矫正这里的GapValue这个除数。
																		//当发现测试出来的重量偏大时，增加该数值。
																		//如果测试出来的重量偏小时，减小改数值。
	}
	else
	{
		Weight_Shiwu = 0;
	}

	LOG_I("Read value:%d, Weight: %dg\r\n", read_value, Weight_Shiwu);
}


void hx711_task_handle(void)
{
	hx711_get_weight();
}
