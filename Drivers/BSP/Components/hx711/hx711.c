#include "stm32f1xx_hal.h"
#include "delay.h"
#include "hx711.h"
#include "board.h"
#include "ulog.h"
#include "system_info.h"


#define HX711_SCK_HIGH()        	(HX711_SCK_GPIO_PORT->BSRR = HX711_SCK_GPIO_PIN)
#define HX711_SCK_LOW()       		(HX711_SCK_GPIO_PORT->BSRR = (uint32_t)(HX711_SCK_GPIO_PIN  << 16u))

#define HX711_DOUT_HIGH()        	(HX711_DOUT_GPIO_PORT->BSRR = HX711_DOUT_GPIO_PIN)
// #define HX711_DOUT_LOW()      		(HX711_DOUT_GPIO_PORT->BSRR = (uint32_t)(HX711_DOUT_GPIO_PIN  << 16u))
#define HX711_DOUT_PIN()			HAL_GPIO_ReadPin(HX711_DOUT_GPIO_PORT, HX711_DOUT_GPIO_PIN)

#define HX711_READ_TIMEOUT		(100000)//100ms
#define HX711_WEIGHT_WUCHA		(0)

static uint32_t zero_value = 0;
static float gap_value = 0;
static uint32_t weight_value = 0;


static uint32_t hx711_read(uint32_t timeout_us)	//增益128
{
	uint32_t wait_num = 0;
	unsigned long count;
	unsigned char i;

  	// HX711_DOUT_HIGH();
	// delay_us(1);
  	HX711_SCK_LOW();//使能AD（PD_SCK 置低）

  	while(HX711_DOUT_PIN())//AD转换未结束则等待，否则开始读取
	{
		if(wait_num * 10 > timeout_us)
		{
			LOG_I("[HX]wait_timeout\r\n");
			return 0;
		}
		delay_us(10);
		wait_num++;
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


void hx711_set_zero(void)
{
	zero_value = hx711_read(HX711_READ_TIMEOUT*10);
	sysinfo_store_hxzero(zero_value);
	LOG_I("[HX]zero read value: %d\r\n", zero_value);
}

void hx711_set_calibration(uint32_t weight)
{
	uint32_t diff_value;
	uint32_t read_value = hx711_read(HX711_READ_TIMEOUT * 10);

	if(read_value > zero_value)
	{
		diff_value = read_value - zero_value;				
		gap_value = ((float)diff_value/weight);
		sysinfo_store_hxgap(gap_value);
		LOG_I("[HX]calibration success w: %d,r: %d,z:%d,gap:%f\r\n", weight, read_value, zero_value, gap_value); 													
	}
	else
	{
		LOG_I("[HX]calibration fail w: %d,r: %d,z:%d,gap:%f\r\n", weight, read_value, zero_value, gap_value); 
	}
}

uint32_t hx711_get_weight(void)
{
	uint32_t read_weight = 0;

	uint32_t diff_value;
	uint32_t read_value = hx711_read(HX711_READ_TIMEOUT);
	if(read_value > zero_value)
	{
		diff_value = read_value - zero_value;				
		read_weight = (uint32_t)((float)diff_value/gap_value);
		if(abs(read_weight - weight_value) > HX711_WEIGHT_WUCHA)
			 weight_value = read_weight; //更新																		
	}
	else
		weight_value = 0;
	// LOG_I("[HX]read value:%d, weight: %dg\r\n", read_value, weight_value);
	return weight_value;
}

uint32_t hx711_get_weight_value(void)
{
	return weight_value;
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

	zero_value = sysinfo_get_hxzero();
	gap_value = sysinfo_get_hxgap();

	LOG_I("[HX]%s zero:%d, gap:%f\r\n", __FUNCTION__, zero_value, gap_value);
}

