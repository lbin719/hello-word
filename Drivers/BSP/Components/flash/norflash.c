#include "norflash.h"
#include "stm32f1xx_hal.h"
#include "ulog.h"
#include "main.h"
#include "spi.h"

#define NORFLASH_USE_DMA         0


uint32_t chip_size = GD25Q16_CHIP_SIZE;
static uint8_t norflash_sector_buf[SPIF_SECTOR_SIZE];

/* NORFLASH 片选信号 */
#define NORFLASH_CS(x)      do{ x ? \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)

static void norflash_wait_busy(void)
{
    uint8_t breadbyte = 0;

    do{
        NORFLASH_CS(0);
        uint8_t  write_data = SPIF_READSTATUSREG1;
        spi2_bytes_write(&write_data, 1);
        spi2_bytes_read(&breadbyte, sizeof(breadbyte));
        NORFLASH_CS(1);
    }while((breadbyte & 0x01) == 0x01);
}

static void norflash_write_enable(void)
{
    NORFLASH_CS(0);
    uint8_t  write_data = SPIF_WRITEENABLE;
    spi2_bytes_write(&write_data, 1);
    NORFLASH_CS(1);
}

static uint16_t norflash_read_id(void)
{
  uint8_t readid_cmd[] = {SPIF_MANUFACTDEVICEID, 0x00, 0x00, 0x00};
  uint8_t wreceivedata[2];

  NORFLASH_CS(0);
  spi2_bytes_write(readid_cmd, sizeof(readid_cmd));
  spi2_bytes_read(wreceivedata, sizeof(wreceivedata));
  NORFLASH_CS(1);

  return (uint16_t)(wreceivedata[0] << 8 | wreceivedata[1]);
}
#if 0
static void norflash_chip_erase(void)
{
    norflash_write_enable();
    norflash_wait_busy();

    NORFLASH_CS(0);
    uint8_t erase_cmd[] = {SPIF_CHIPERASE};
    spi2_bytes_write(erase_cmd, sizeof(erase_cmd));
    NORFLASH_CS(1);

    norflash_wait_busy();
}
#endif
static void norflash_sector_erase(uint32_t erase_addr)
{
    uint8_t erase_data[4];

    norflash_write_enable();
    norflash_wait_busy();

    erase_addr *= SPIF_SECTOR_SIZE; /* translate sector address to byte address */

    erase_data[0] = SPIF_SECTORERASE;
    erase_data[1] = (uint8_t)(erase_addr >> 16); /* send 24-bit address */
    erase_data[2] = (uint8_t)(erase_addr >> 8);
    erase_data[3] = (uint8_t)erase_addr;

    NORFLASH_CS(0);
    spi2_bytes_write(erase_data, sizeof(erase_data));
    NORFLASH_CS(1);
    norflash_wait_busy();
}

static void norflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
  if((0 < length) && (length <= SPIF_PAGE_SIZE))
  {
    /* set write enable */
    norflash_write_enable();

    NORFLASH_CS(0);

    /* send instruction */
    /* send 24-bit address */
    uint8_t write_cmd_addr[4];
    write_cmd_addr[0] = SPIF_PAGEPROGRAM;
    write_cmd_addr[1] = (uint8_t)(write_addr >> 16);
    write_cmd_addr[2] = (uint8_t)(write_addr >> 8);
    write_cmd_addr[3] = (uint8_t)write_addr;
    spi2_bytes_write(write_cmd_addr, sizeof(write_cmd_addr));
#if NORFLASH_USE_DMA
    spi2_dma_write(pbuffer, length);
    spi2_dma_wait_finsh();
#else
    spi2_bytes_write(pbuffer, length);
#endif
    NORFLASH_CS(1);

    /* wait for program end */
    norflash_wait_busy();
  }
}

static void norflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
    uint16_t page_remain;

    /* remain bytes in a page */
    page_remain = SPIF_PAGE_SIZE - write_addr % SPIF_PAGE_SIZE;
    if(length <= page_remain)
    {
        /* smaller than a page size */
        page_remain = length;
    }
    while(1)
    {
        norflash_page_write(pbuffer, write_addr, page_remain);
        if(length == page_remain)
        {
            /* all data are programmed */
            break;
        }
        else
        {
            /* length > page_remain */
            pbuffer += page_remain;
            write_addr += page_remain;

            /* the remain bytes to be prorammed */
            length -= page_remain;
            if(length > SPIF_PAGE_SIZE)
            {
                /* can be progrmmed a page at a time */
                page_remain = SPIF_PAGE_SIZE;
            }
            else
            {
                /* smaller than a page size */
                page_remain = length;
            }
        }
    }
}


void norflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
    uint32_t sector_pos;
    uint16_t sector_offset;
    uint16_t sector_remain;
    uint16_t index;
    uint8_t *norflash_buf;

    // LOG_I("norflash_write addr: 0x%08x, len:%d\r\n", write_addr, length);
    if((write_addr + length) >= SPIF_CHIP_SIZE)
    {
        return ;
    }

    norflash_buf = norflash_sector_buf;

    /* sector address */
    sector_pos = write_addr / SPIF_SECTOR_SIZE;

    /* address offset in a sector */
    sector_offset = write_addr % SPIF_SECTOR_SIZE;

    /* the remain in a sector */
    sector_remain = SPIF_SECTOR_SIZE - sector_offset;
    if(length <= sector_remain)
    {
        /* smaller than a sector size */
        sector_remain = length;
    }

    while(1)
    {
        /* read a sector */
        norflash_read(norflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE);

        /* validate the read erea */
        for(index = 0; index < sector_remain; index++)
        {
            if(norflash_buf[sector_offset + index] != 0xFF)
            {
                /* there are some data not equal 0xff, so this secotr needs erased */
                break;
            }
        }
        if(index < sector_remain)
        {
            /* erase the sector */
            norflash_sector_erase(sector_pos);

            /* copy the write data */
            for(index = 0; index < sector_remain; index++)
            {
                norflash_buf[index + sector_offset] = pbuffer[index];
            }
            norflash_write_nocheck(norflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE); /* program the sector */
        }
        else
        {
            /* write directly in the erased area */
            norflash_write_nocheck(pbuffer, write_addr, sector_remain);
        }
        if(length == sector_remain)
        {
            /* write end */
            break;
        }
        else
        {
            /* go on writing */
            sector_pos++;
            sector_offset = 0;

            pbuffer += sector_remain;
            write_addr += sector_remain;
            length -= sector_remain;
            if(length > SPIF_SECTOR_SIZE)
            {
            /* could not write the remain data in the next sector */
            sector_remain = SPIF_SECTOR_SIZE;
        }
        else
        {
            /* could write the remain data in the next sector */
            sector_remain = length;
        }
        }
    }
}

void norflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length)
{
    uint8_t read_data[4];

    // LOG_I("norflash_read addr: 0x%08x, len:%d\r\n", read_addr, length);
    if((read_addr + length) >= SPIF_CHIP_SIZE)
    {
        return ;
    }

    read_data[0] = SPIF_READDATA;
    read_data[1] = (uint8_t)(read_addr >> 16); /* send 24-bit address */
    read_data[2] = (uint8_t)(read_addr >> 8);
    read_data[3] = (uint8_t)read_addr;

    NORFLASH_CS(0);
    spi2_bytes_write(read_data, sizeof(read_data));
#if NORFLASH_USE_DMA
    spi2_dma_read(pbuffer, length);
    spi2_dma_wait_finsh();
#else
    spi2_bytes_read(pbuffer, length);
#endif

    NORFLASH_CS(1);
}


bool norflash_init(void)
{
    NORFLASH_GPIO_CLK_ENABLE();      /* spi2 CS脚 时钟使能 */
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = NORFLASH_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NORFLASH_CS_GPIO_PORT, &gpio_init_struct);
    NORFLASH_CS(1);

    spi2_init();

    uint16_t flash_id = norflash_read_id();
    if(GD25Q16 == flash_id)
    {
        chip_size = GD25Q16_CHIP_SIZE;
        LOG_I("flash id:%x is:%s size:%x\r\n", flash_id, "GD25Q16", chip_size);
        return true;
    }
    else if(WB25Q64 == flash_id)
    {
        chip_size = WB25Q64_CHIP_SIZE;
        LOG_I("flash id:%x is:%s size:%x\r\n", flash_id, "WB25Q64", chip_size);
        return true;
    }
    else if(WB25Q128 == flash_id)
    {
        chip_size = WB25Q128_CHIP_SIZE;
        LOG_I("flash id:%x is:%s size:%x\r\n", flash_id, "WB25Q128", chip_size);
        return true;
    }
    else
    {
        LOG_I("flash init fail id: %x\r\n", flash_id);
        return false;
    }
}

void flash_test(void)
{
    #define TEST_SIZE   256
    uint8_t write_buffer[256];

    while(1)
    {
        uint32_t read_addr = 0;
        uint32_t i = 0;

        norflash_sector_erase(read_addr/SPIF_SECTOR_SIZE);
        for(i = 0; i < 256; i++)
        {
            write_buffer[i] = i;//i * i ^ i;
        }
        norflash_page_write(write_buffer, read_addr, sizeof(write_buffer));

        norflash_read(norflash_sector_buf, read_addr, SPIF_SECTOR_SIZE);
        LOG_I("read addr: 0x%08x, data:\r\n", read_addr);
        for(i = 0; i < 128; i++)
        {
            LOG_I_NOTICK("%02x ", norflash_sector_buf[i]);
        }
        LOG_I_NOTICK("\r\n");

        /* 比较数据 */
        for (i = 0; i < 256; i++)
        {
            if (norflash_sector_buf[i] != write_buffer[i])
            {
                LOG_I("data rw err! w(%x) != r(%x)\r\n", write_buffer[i], norflash_sector_buf[i]);
                break;
            }
        }
        if(i == 256)
        {
            LOG_I("data rw correct\r\n");
        }

        HAL_Delay(2000);
    }
}
