#include "stmflash.h"
#include "stm32f1xx_hal.h"
#include "ulog.h"

uint32_t stmflash_write(uint32_t waddr, uint32_t* pbuf ,uint32_t length)
{
    if (waddr < STM32_FLASH_BASE || (waddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* �Ƿ���ַ */
    }
    if (waddr % 4)
    {
        return FLASHIF_WRITING_ERROR;     /* �Ƿ���ַ */
    }

    HAL_FLASH_Unlock();     /* FLASH���� */
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
                HAL_FLASH_Lock(); /* ���� */
                return(FLASHIF_WRITINGCTRL_ERROR);
            }
            /* Increment FLASH destination address */
            waddr += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            HAL_FLASH_Lock(); /* ���� */
            return (FLASHIF_WRITING_ERROR);
        }
    }
    HAL_FLASH_Lock(); /* ���� */
    return (FLASHIF_OK);
}

uint32_t stmflash_erase(uint32_t addr, uint32_t size)
{
    FLASH_EraseInitTypeDef flash_eraseop;
    uint32_t erase_addr;    /* �����������ֵΪ���������������ַ */

    if (addr < STM32_FLASH_BASE || (addr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE)))
    {
        return FLASHIF_WRITING_ERROR;     /* �Ƿ���ַ */
    }

    if ((addr % STM32_FLASH_SIZE) || (size % STM32_FLASH_SIZE))
    {
        return FLASHIF_WRITING_ERROR;     /* �Ƿ���ַ */
    }

    flash_eraseop.TypeErase = FLASH_TYPEERASE_PAGES;     /* ѡ��ҳ���� */
    flash_eraseop.NbPages = size / STM32_FLASH_SIZE;
    flash_eraseop.PageAddress = addr;  /* Ҫ���������� */

    HAL_FLASH_Unlock(); /* FLASH���� */
    HAL_FLASHEx_Erase( &flash_eraseop, &erase_addr);
    HAL_FLASH_Lock(); /* ���� */

    return FLASHIF_OK;
}

void stmflash_test(void)
{
  uint32_t addr = STM32_FLASH_END_ADDRESS - STM32_FLASH_SIZE;
  stmflash_erase(addr, STM32_FLASH_SIZE);
  uint32_t w_b[] = {0,1,2,3,4,5,6,7};
  stmflash_write(addr, w_b, sizeof(w_b));

  uint32_t *p = (uint32_t *)addr;
  LOG_I("w:%d,%d,%d,%d", p[0], p[1], p[2], p[3] );
}

