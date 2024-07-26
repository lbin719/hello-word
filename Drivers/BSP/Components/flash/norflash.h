#ifndef __NORFLASH_H
#define __NORFLASH_H
#include "stdint.h"
#include "stdbool.h"

/*
 * flash define
 */
#define GD25Q16                          0xC814
#define WB25Q64                          0xEF16
/** @defgroup SPI_flash_operation_definition
  * @{
  */
#define GD25Q16_CHIP_SIZE                (0x200000)
#define WB25Q64_CHIP_SIZE                (0x800000)

#define SPIF_CHIP_SIZE                   (chip_size)

#define SPIF_SECTOR_SIZE                 (4096)
#define SPIF_PAGE_SIZE                   (256)


#define SPIF_WRITEENABLE                 0x06
#define SPIF_WRITEDISABLE                0x04
/* s7-s0 */
#define SPIF_READSTATUSREG1              0x05
#define SPIF_WRITESTATUSREG1             0x01
/* s15-s8 */
#define SPIF_READSTATUSREG2              0x35
#define SPIF_WRITESTATUSREG2             0x31
/* s23-s16 */
#define SPIF_READSTATUSREG3              0x15
#define SPIF_WRITESTATUSREG3             0x11
#define SPIF_READDATA                    0x03
#define SPIF_FASTREADDATA                0x0B
#define SPIF_FASTREADDUAL                0x3B
#define SPIF_PAGEPROGRAM                 0x02
/* block size:64kb */
#define SPIF_BLOCKERASE                  0xD8
#define SPIF_SECTORERASE                 0x20
#define SPIF_CHIPERASE                   0xC7
#define SPIF_POWERDOWN                   0xB9
#define SPIF_RELEASEPOWERDOWN            0xAB
#define SPIF_DEVICEID                    0xAB
#define SPIF_MANUFACTDEVICEID            0x90
#define SPIF_JEDECDEVICEID               0x9F
#define FLASH_SPI_DUMMY_BYTE             0xA5

extern uint32_t chip_size;

bool norflash_init(void);
void norflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
void norflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length);

#endif
