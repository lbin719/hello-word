/**
  ******************************************************************************
  * @file    flash_diskio.h
  * @author  MCD Application Team
  * @brief   Header for flash_diskio.c module.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_DISKIO_H
#define __FLASH_DISKIO_H
#include "ff_gen_drv.h"
#include "norflash.h"

#define FLASH_DISK_BLOCK_SIZE    8 /* 每个 BLOCK 有 8 个扇区 */
#define FLASH_DISK_SECTOR_SIZE   (SPIF_SECTOR_SIZE / FLASH_DISK_BLOCK_SIZE) /* 扇区大小 */
#define FLASH_DISK_SECTOR_COUNT  (SPIF_CHIP_SIZE / FLASH_DISK_SECTOR_SIZE) /* 扇区数目 */

extern const Diskio_drvTypeDef  FLASHDISK_Driver;

#endif /* __FLASH_DISKIO_H */
