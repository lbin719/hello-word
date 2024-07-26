#include "stmflash.h"
#include "stm32f1xx_hal.h"

uint32_t stmflash_write(uint32_t waddr, uint32_t* pbuf ,uint32_t length)
{
    if (waddr < STM32_FLASH_BASE || (waddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* 非法地址 */
    }
    if (waddr % 4)
    {
        return FLASHIF_WRITING_ERROR;     /* 非法地址 */
    }

    HAL_FLASH_Unlock();     /* FLASH解锁 */
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
                HAL_FLASH_Lock(); /* 上锁 */
                return(FLASHIF_WRITINGCTRL_ERROR);
            }
            /* Increment FLASH destination address */
            waddr += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            HAL_FLASH_Lock(); /* 上锁 */
            return (FLASHIF_WRITING_ERROR);
        }
    }
    HAL_FLASH_Lock(); /* 上锁 */
    return (FLASHIF_OK);
}

uint32_t stmflash_erase(uint32_t addr, uint32_t size)
{
    FLASH_EraseInitTypeDef flash_eraseop;
    uint32_t erase_addr;    /* 擦除错误，这个值为发生错误的扇区地址 */

    if (addr < STM32_FLASH_BASE || (addr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* 非法地址 */
    }

    if ((addr % STM32_FLASH_SIZE) || (size % STM32_FLASH_SIZE))
    {
        return FLASHIF_WRITING_ERROR;     /* 非法地址 */
    }

    flash_eraseop.TypeErase = FLASH_TYPEERASE_PAGES;     /* 选择页擦除 */
    flash_eraseop.NbPages = size / STM32_FLASH_SIZE;
    flash_eraseop.PageAddress = addr;  /* 要擦除的扇区 */

    HAL_FLASH_Unlock(); /* FLASH解锁 */
    HAL_FLASHEx_Erase( &flash_eraseop, &erase_addr);
    HAL_FLASH_Lock(); /* 上锁 */

    return FLASHIF_OK;
}

void stmflash_test(void)
{
  uint32_t addr = STM32_FLASH_END_ADDRESS - STM32_FLASH_SIZE;
  stmflash_erase(addr, STM32_FLASH_SIZE);
  uint8_t w_b[] = {0,1,2,3,4,5,6,7};
  stmflash_write(addr, w_b, sizeof(w_b));

  uint8_t *p = addr;
  LOG_I("w:%d,%d,%d,%d", w_b[0], w_b[1], w_b[2], w_b[3] );
}