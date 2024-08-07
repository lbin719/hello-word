#include "stm32f1xx_hal.h"
#include "delay.h"
#include "hx711.h"
#include "board.h"


#define HX711_SCK_HIGH()        	(HX711_SCK_GPIO_PORT->BSRR = HX711_SCK_GPIO_PIN)
#define HX711_SCK_LOW()       		(HX711_SCK_GPIO_PORT->BSRR = (uint32_t)(HX711_SCK_GPIO_PIN  << 16u))

#define HX711_DOUT_HIGH()        	(HX711_DOUT_GPIO_PORT->BSRR = HX711_DOUT_GPIO_PIN)
// #define HX711_DOUT_LOW()      		(HX711_DOUT_GPIO_PORT->BSRR = (uint32_t)(HX711_DOUT_GPIO_PIN  << 16u))
#define HX711_DOUT_PIN()			HAL_GPIO_ReadPin(HX711_DOUT_GPIO_PORT, HX711_DOUT_GPIO_PIN)


uint32_t Weight_Maopi;
int32_t Weight_Shiwu;


//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
#define GapValue 106.5


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
}

//****************************************************
//��ȡHX711
//****************************************************
uint32_t hx711_read(void)	//����128
{
	unsigned long count;
	unsigned char i;

  	// HX711_DOUT_HIGH();
	// delay_us(1);
  	HX711_SCK_LOW();//ʹ��AD��PD_SCK �õͣ�

  	while(HX711_DOUT_PIN());//ADת��δ������ȴ�������ʼ��ȡ

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
    count = count ^ 0x800000;//��25�������½�����ʱ��ת������
	delay_us(1);
	HX711_SCK_LOW();
	return(count);
}

//****************************************************
//��ȡëƤ����
//****************************************************
void hx711_get_maopi(void)
{
	Weight_Maopi = hx711_read();
}

//****************************************************
//����
//****************************************************
void hx711_get_weight(void)
{
	uint32_t read_value;

	read_value = hx711_read();
	if(read_value > Weight_Maopi)
	{
		Weight_Shiwu = read_value;
		Weight_Shiwu = Weight_Shiwu - Weight_Maopi;				//��ȡʵ���AD������ֵ��

		Weight_Shiwu = (int32_t)((float)Weight_Shiwu/GapValue); 	//����ʵ���ʵ������
																		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������GapValue���������
																		//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
																		//������Գ���������ƫСʱ����С����ֵ��
	}
}
