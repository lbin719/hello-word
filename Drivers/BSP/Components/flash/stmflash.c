#include "stmflash.h"
#include "stm32f1xx_hal.h"
#include "ulog.h"

uint32_t stmflash_write(uint32_t waddr, uint32_t *pbuf ,uint32_t length)
{
    LOG_I("%s addr:0x%08x length:0x%x\r\n",  __func__, waddr, length);

    if (waddr < STM32_FLASH_BASE || ((waddr + length) > (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* 闈炴硶鍦板潃 */
    }
    if (waddr % 4)
    {
        return FLASHIF_WRITING_ERROR;     /* 闈炴硶鍦板潃 */
    }

    HAL_FLASH_Unlock();     /* FLASH瑙ｉ攣 */
    for (uint32_t i = 0; (i < length) && (waddr <= (STM32_FLASH_END_ADDRESS - 4)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
        be done by word */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, waddr, *(uint32_t*)(pbuf+i)) == HAL_OK)
        {
            /* Check the written value */
            if (*(uint32_t*)waddr != *(uint32_t*)(pbuf+i))
            {
                /* Flash content doesn't match SRAM content */
                HAL_FLASH_Lock(); /* 涓婇攣 */
                return(FLASHIF_WRITINGCTRL_ERROR);
            }
            /* Increment FLASH destination address */
            waddr += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            HAL_FLASH_Lock(); /* 涓婇攣 */
            return (FLASHIF_WRITING_ERROR);
        }
    }
    HAL_FLASH_Lock(); /* 涓婇攣 */
    return (FLASHIF_OK);
}

uint32_t stmflash_erase(uint32_t addr, uint32_t size)
{
    FLASH_EraseInitTypeDef flash_eraseop;
    uint32_t erase_addr;    /* 鎿﹂櫎閿欒?锛岃繖涓?€间负鍙戠敓閿欒?鐨勬墖鍖哄湴鍧€ */

    LOG_I("%s addr:0x%08x size:0x%x\r\n", __func__, addr, size);

    if (addr < STM32_FLASH_BASE || ((addr + size) > (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* 闈炴硶鍦板潃 */
    }

    // if ((addr % STM32_SECTOR_SIZE) || (size % STM32_SECTOR_SIZE))
    // {
    //     return FLASHIF_WRITING_ERROR;     /* 闈炴硶鍦板潃 */
    // }

    flash_eraseop.TypeErase = FLASH_TYPEERASE_PAGES;     /* 閫夋嫨椤垫摝闄? */
    flash_eraseop.NbPages = size / STM32_SECTOR_SIZE  + ((size % STM32_SECTOR_SIZE) ? 1 : 0);
    flash_eraseop.PageAddress = addr;  /* 瑕佹摝闄ょ殑鎵囧尯 */

    HAL_FLASH_Unlock(); /* FLASH瑙ｉ攣 */
    HAL_FLASHEx_Erase( &flash_eraseop, &erase_addr);
    HAL_FLASH_Lock(); /* 涓婇攣 */

    return FLASHIF_OK;
}

#if 0
void stmflash_test(void)
{
     uint32_t ret = FLASHIF_OK;
     uint32_t addr = STM32_FLASH_END_ADDRESS - STM32_SECTOR_SIZE;

     ret = stmflash_erase(addr, STM32_SECTOR_SIZE);
     if(ret != FLASHIF_OK)
     {
         LOG_I("stmflash_erase error ret:%d\r\n", ret);
     }

     uint8_t w_b[] = {0,1,2,3,4,5,6,7};
     ret = stmflash_write(addr, w_b, sizeof(w_b));
     if(ret != FLASHIF_OK)
     {
         LOG_I("stmflash_write error ret:%d\r\n", ret);
     }

     uint8_t *p = (uint8_t *)addr;
     LOG_I("addr:0x%08x data: %d,%d,%d,%d\r\n", addr, p[0], p[1], p[2], p[3] );
}
#endif
