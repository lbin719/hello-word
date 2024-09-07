#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "stdint.h"

/* FLASH起始地址 */
#define STMFLASH_SIZE                   (0x40000)         /* STM32 FLASH 总大小 256k */
#define STMFLASH_BASE                   (0x08000000)      /* STM32 FLASH 起始地址 */
#define STMFLASH_END_ADDRESS            (STMFLASH_BASE + STMFLASH_SIZE)

 /* STM32F103 扇区大小 */
#if STMFLASH_SIZE < 256 * 1024
#define STM32_SECTOR_SIZE   1024                /* 容量小于256K的 F103, 扇区大小为1K字节 */
#else
#define STM32_SECTOR_SIZE   2048                /* 容量大于等于256K的 F103, 扇区大小为2K字节 */
#endif


#define BOOT_FW_START_ADDR                (STMFLASH_BASE)
#define BOOT_FW_SIZE                      (40 * 1024)

#define APP_FW_START_ADDR                 (BOOT_FW_START_ADDR + BOOT_FW_SIZE)
#define APP_FW_SIZE                       (200 * 1024)

//priv
#define STM32_PRIV_START_ADDR             (STMFLASH_END_ADDRESS - 4 * STM32_SECTOR_SIZE)


//System data

#define STMFLASH_SYSINFO_START_ADDR    (STM32_PRIV_START_ADDR)
#define STMFLASH_SYSINFO_SIZE          (STM32_SECTOR_SIZE)

//key
#define STMFLASH_EN_ID_START_ADDR      (STMFLASH_SYSINFO_START_ADDR + STMFLASH_SYSINFO_SIZE + STM32_SECTOR_SIZE)
#define STMFLASH_EN_ID_SIZE            (STM32_SECTOR_SIZE)

//nvms
#define STMFLASH_NVMS_START_ADDR       (STMFLASH_EN_ID_START_ADDR + STMFLASH_EN_ID_SIZE)
#define STMFLASH_NVMS_SIZE             (STM32_SECTOR_SIZE)


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

uint32_t stmflash_read(uint32_t addr, uint8_t *data, uint32_t size);

#endif


















