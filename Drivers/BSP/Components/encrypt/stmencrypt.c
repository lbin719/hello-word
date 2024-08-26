#include "stmencrypt.h"
#include "stmflash.h"
#include "ulog.h"

typedef struct {
	uint32_t header;
	uint32_t tick;
    uint32_t key;
} stmencrypt_data_t;

const uint32_t trng_data[4] = {0xCBE6BBFA, 0xCAFDB8F6, 0xCAFDA3BA, 0x3130300D0A};
#define STMENCRYPT_HEADER   0x56F3B4A6
#define STM32_ID_D  848468   //任意的一个数

//stm32芯片的ID地址，把地址减去一个数，避免汇编里面直接出现ID的地址，不然很容易暴露加密与ID号有关
volatile uint32_t stm32_id_addr[]={
    UID_BASE - STM32_ID_D,
    UID_BASE + 4 + STM32_ID_D,
    UID_BASE + 8 - STM32_ID_D
};

bool stmencrypt_status = false;

volatile void stm_read_uuid(volatile uint32_t *p)
{
   volatile uint32_t Addr;
// 因为不想让程序在反汇编后直接找到这个地址，所以这个地址是运算出来的，
// 跟stm32_id_addr反运算，当然了也可以用高级的算法，注意不能让编译器优化这个地址
   Addr = stm32_id_addr[0] + STM32_ID_D;
   p[0] = *(uint32_t*)(Addr);
   Addr = stm32_id_addr[1] - STM32_ID_D;
   p[1] = *(uint32_t*)(Addr);
   Addr = stm32_id_addr[2] + STM32_ID_D;
   p[2] = *(uint32_t*)(Addr);
}

uint32_t stm_generate_key(void)
{
   uint32_t stm32ID[4], dat;
   stm_read_uuid(stm32ID);
   stm32ID[3] = STM32_ID_D;

   //这里可以用其它一些高级的算法，但解和加要一样
   //把ID号处理成一个32位数，也可以用自己的算法，处理成其他数据，多少位都行
   dat = (stm32ID[0] ^ stm32ID[1]) - (stm32ID[2] / stm32ID[3]); //处理成一个32位数
   dat &= trng_data[0]; 
   dat |= trng_data[1];
   dat ^= trng_data[2];  
   dat ^= trng_data[3]; 
//    LOG_I("id: 0x%08x 0x%08x 0x%08x 0x%08x key: 0x%08x\r\n", stm32ID[0], stm32ID[1], stm32ID[2], stm32ID[3], dat);
   return dat;
}

/********************************************************************
函数功能：加密ID并保存
入口参数：
返    回：
备    注：
********************************************************************/
void stmencrypt_write_key(void)
{
    uint32_t ret = FLASHIF_OK;
    uint32_t flash_key = *(uint32_t*)(STM32_FLASH_EN_ID_START_ADDR);  //读出加密时，保存在flash中的数
    // if(flash_key == 0xFFFFFFFF)//未写入
    {
        stmencrypt_data_t ststmencrypt_data;
        ststmencrypt_data.header = STMENCRYPT_HEADER;
        ststmencrypt_data.tick = SysTick->VAL;
        ststmencrypt_data.key = stm_generate_key();
        ststmencrypt_data.key ^= ststmencrypt_data.tick;

        stmflash_erase(STM32_FLASH_EN_ID_START_ADDR, STM32_FLASH_EN_ID_SIZE);
        if(ret != FLASHIF_OK)
        {
            LOG_I("stmflash_erase error ret:%d\r\n", ret);
        }
        stmflash_write(STM32_FLASH_EN_ID_START_ADDR, &ststmencrypt_data, sizeof(stmencrypt_data_t)); //保存这个数，写进32位
        if(ret != FLASHIF_OK)
        {
            LOG_I("stmflash_write error ret:%d\r\n", ret);
        }
        //check
//        uint32_t read_key = *(uint32_t*)(STM32_FLASH_EN_ID_START_ADDR);  //读出加密时，保存在flash中的数
//        if(read_key != key)
//        {
//            LOG_I("write key fail. key: 0x%08x, read_key: 0x%08x\r\n", key, read_key);
//        }
    }
}

/********************************************************************
函数功能：比较加密ID，正确返回0
入口参数：
返    回：1：不正确，0：正确
备    注：
********************************************************************/
bool stmencrypt_cmp_key(void)
{
    uint32_t calculater_key = 0;
    stmencrypt_data_t ststmencrypt_data;
    memcpy((uint8_t *)&ststmencrypt_data, (uint8_t *)(STM32_FLASH_EN_ID_START_ADDR), sizeof(ststmencrypt_data));
    // uint32_t flash_key = *(uint32_t*)(STM32_FLASH_EN_ID_START_ADDR);  //读出加密时，保存在flash中的数

    if(ststmencrypt_data.header != STMENCRYPT_HEADER)//  未写入
    {
        return false;
    }

    calculater_key = stm_generate_key();
    calculater_key ^= ststmencrypt_data.tick;

    if(ststmencrypt_data.key == calculater_key)
    {
        // LOG_I("stmencrypt success. key: 0x%08x\r\n", key);     // ID正确，正常运行
        return true;
    }
    else
    {
        // LOG_I("stmencrypt fail. flash_key: 0x%08x, key: 0x%08x\r\n", flash_key, key);     // ID正确，正常运行
        return false;
    }
}

bool get_stmencrypt_status(void)
{
    return stmencrypt_status;
}

void stmencrypt_init(void)
{
    stmencrypt_status = stmencrypt_cmp_key();

    LOG_I("%s result: %s\r\n", __FUNCTION__, stmencrypt_status ? "false" : "true");
}

