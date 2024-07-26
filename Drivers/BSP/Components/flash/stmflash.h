#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "stdint.h"

/* FLASH起始地址 */
#define STM32_FLASH_SIZE        0x10000         /* STM32 FLASH 总大小 */
#define STM32_FLASH_BASE        0x08000000      /* STM32 FLASH 起始地址 */
#define STM32_FLASH_END_ADDRESS (0x08000000 + 0x80000)

 /* STM32F103 扇区大小 */
#if STM32_FLASH_SIZE < 256 * 1024
#define STM32_SECTOR_SIZE   1024                /* 容量小于256K的 F103, 扇区大小为1K字节 */
#else
#define STM32_SECTOR_SIZE   2048                /* 容量大于等于于256K的 F103, 扇区大小为2K字节 */
#endif

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


















