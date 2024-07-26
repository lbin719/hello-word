/**
  ******************************************************************************
  * @file    flash_diskio.c
  * @author  MCD Application Team
  * @brief   FLASH Disk I/O driver.
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

/* Includes ------------------------------------------------------------------*/
#include "norflash_diskio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block Size in Bytes */

#ifndef __I
#define	__I     volatile const       /*!< Defines 'read only' permissions */
#endif
#ifndef __O
#define	__O     volatile             /*!< Defines 'write only' permissions */
#endif
#ifndef __IO
#define	__IO    volatile             /*!< Defines 'read / write' permissions */
#endif

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
DSTATUS FLASHDISK_initialize (BYTE);
DSTATUS FLASHDISK_status (BYTE);
DRESULT FLASHDISK_read (BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
  DRESULT FLASHDISK_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
  DRESULT FLASHDISK_ioctl (BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef FLASHDISK_Driver =
{
  FLASHDISK_initialize,
  FLASHDISK_status,
  FLASHDISK_read,
#if  _USE_WRITE == 1
  FLASHDISK_write,
#endif /* _USE_WRITE == 1 */
#if  _USE_IOCTL == 1
  FLASHDISK_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS FLASHDISK_initialize(BYTE lun)
{
  Stat = STA_NOINIT;

//  if(norflash_init())
  {
     Stat &= ~STA_NOINIT;
  }

  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
DSTATUS FLASHDISK_status(BYTE lun)
{
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT FLASHDISK_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    norflash_read(buff, sector * FLASH_DISK_SECTOR_SIZE, count * FLASH_DISK_SECTOR_SIZE);
    return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : not used
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT FLASHDISK_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    norflash_write((uint8_t*)buff, sector * FLASH_DISK_SECTOR_SIZE, count * FLASH_DISK_SECTOR_SIZE);
    return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  lun : not used
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT FLASHDISK_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (cmd)
    {
        /* Make sure that no pending write process */
        case CTRL_SYNC :
        res = RES_OK;
        break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT :
        *(DWORD*)buff = FLASH_DISK_SECTOR_COUNT;
        res = RES_OK;
        break;

        /* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE :
        *(DWORD*)buff = FLASH_DISK_SECTOR_SIZE;
        res = RES_OK;
        break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE :
        *(DWORD*)buff = FLASH_DISK_BLOCK_SIZE;
        res = RES_OK;
        break;

        default: res = RES_ERROR;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */
