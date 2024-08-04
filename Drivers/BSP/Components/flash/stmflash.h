#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "stdint.h"

/* FLASH��ʼ��ַ */
#define STM32_FLASH_SIZE        0x40000         /* STM32 FLASH �ܴ�С */
#define STM32_FLASH_BASE        0x08000000      /* STM32 FLASH ��ʼ��ַ */
#define STM32_FLASH_END_ADDRESS (0x08000000 + 0x80000)

 /* STM32F103 ������С */
#if STM32_FLASH_SIZE < 256 * 1024
#define STM32_SECTOR_SIZE   1024                /* ����С��256K�� F103, ������СΪ1K�ֽ� */
#else
#define STM32_SECTOR_SIZE   2048                /* �������ڵ���256K�� F103, ������СΪ2K�ֽ� */
#endif


#define STM32_FLASH_EN_ID_START_ADDR    (STM32_FLASH_END_ADDRESS - 2*STM32_SECTOR_SIZE)
#define STM32_FLASH_EN_ID_SIZE          (STM32_SECTOR_SIZE) 

#define STM32_FLASH_NVMS_START_ADDR     (STM32_FLASH_EN_ID_START_ADDR + STM32_FLASH_EN_ID_SIZE)
#define STM32_FLASH_NVMS_SIZE           (STM32_SECTOR_SIZE)


/* Error code */
enum
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR
};

uint32_t stmflash_write(uint32_t waddr, uint32_t* pbuf ,uint32_t length);
uint32_t stmflash_erase(uint32_t addr, uint32_t size);

#endif


















