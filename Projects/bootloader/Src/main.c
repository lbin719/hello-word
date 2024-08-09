/**
  ******************************************************************************
  * @file    HID_Standalone/Src/main.c
  * @author  MCD Application Team
  * @brief   USB device HID application main file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ulog.h"
#include "usbd_storage.h"
#include "norflash.h"
#include "stmflash.h"
#include "norflash_diskio.h"
#include "crypto.h"


USBD_HandleTypeDef USBD_Device;
extern volatile uint8_t g_usb_state_reg;
extern volatile uint8_t g_device_state;


void SystemClock_Config(void);

void board_init(void)
{
  GPIO_InitTypeDef gpio_init_struct = {0};

  LED_GREEN_GPIO_CLK_ENABLE();
  LED_BLUE_GPIO_CLK_ENABLE();
  KEY_GPIO_CLK_ENABLE();

  gpio_init_struct.Pin = LED_GREEN_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_struct.Pull = GPIO_NOPULL;
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LED_GREEN_GPIO_PORT, &gpio_init_struct);
  HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN, GPIO_PIN_SET);

  gpio_init_struct.Pin = LED_BLUE_GPIO_PIN;
  HAL_GPIO_Init(LED_BLUE_GPIO_PORT, &gpio_init_struct);
  HAL_GPIO_WritePin(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN, GPIO_PIN_SET);

  gpio_init_struct.Pin = KEY_GPIO_PIN;
  gpio_init_struct.Mode = GPIO_MODE_INPUT;
  gpio_init_struct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_GPIO_PORT, &gpio_init_struct);
}

#define FLASH_SECTOR_SIZE       (2048)
#define APP_FW_CRC_SIZE         (4)

static uint8_t  read_buf[FLASH_SECTOR_SIZE];

static bool firmware_upgrade(uint32_t addr)
{
  FATFS fs;
  FIL file;
  char DISKPath[4];
  uint32_t firmware_size;
  uint32_t firmware_crc32;
  uint32_t read_remain;
  uint32_t read_bytes, read_words;
  uint32_t crc_calcu;
  bool result = true;

  if(FATFS_LinkDriver(&FLASHDISK_Driver, DISKPath) != 0)
  {
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  if(FR_OK != f_mount(&fs, (TCHAR const *)DISKPath, 0))
  {
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  if(FR_OK != f_open(&file, "smartscale_raw.bin", FA_READ))
  {
    f_close(&file);
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  firmware_size = f_size(&file);
  if((firmware_size == 0) || (firmware_size > APP_FW_SIZE))
  {
    f_close(&file);
    f_unlink("smartscale_raw.bin.invalid");
    f_rename("smartscale_raw.bin", "smartscale_raw.bin.invalid");
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  read_remain = firmware_size - APP_FW_CRC_SIZE;
  crc_calcu = 0;
  while(read_remain)
  {
    f_read(&file, read_buf, MIN(read_remain, sizeof(read_buf)), (UINT *)&read_bytes);
    crc_calcu = crypto_crc32_calc(crc_calcu, read_buf, read_bytes);
    read_remain -= read_bytes;
  }
  f_read(&file, (uint8_t*)&firmware_crc32, APP_FW_CRC_SIZE, (UINT *)&read_bytes);
  if((read_bytes != APP_FW_CRC_SIZE) || (firmware_crc32 != crc_calcu))
  {
    //对文件校验失败
    f_close(&file);
    f_unlink("smartscale_raw.bin.invalid");
    f_rename("smartscale_raw.bin", "smartscale_raw.bin.invalid");
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  read_remain = firmware_size - APP_FW_CRC_SIZE;
  crc_calcu = crypto_crc32_calc(0, (uint8_t*)(APP_FW_START_ADDR), read_remain);// 读出当前固件计算crc32
  if(firmware_crc32 == crc_calcu)
  {
    // 不需要更新固件
    f_close(&file);
    FATFS_UnLinkDriver(DISKPath);
    return true;
  }

  //update fw
  stmflash_erase(APP_FW_START_ADDR, firmware_size);

  uint32_t write_addr = addr;
  f_lseek(&file, 0);
  while(read_remain)
  {
    f_read(&file, read_buf, MIN(read_remain, sizeof(read_buf)), (UINT *)&read_bytes);
    read_words = (read_bytes % 4) ? (read_bytes / 4) : ((read_bytes + 3) / 4);
    stmflash_write(write_addr, (uint32_t *)read_buf, read_words);
    write_addr += read_bytes;
    read_remain -= read_bytes;
  }

  //crc check
  read_remain = firmware_size - APP_FW_CRC_SIZE;
  crc_calcu = crypto_crc32_calc(0, (uint8_t*)(APP_FW_START_ADDR), read_remain);// 读出当前固件计算crc32
  if(firmware_crc32 != crc_calcu)
  {
    result = false;//更新失败
  }

	f_close(&file);
  FATFS_UnLinkDriver(DISKPath);

  return result;
}

void jump_to_app(uint32_t address)
{
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//disable systick

	void (*pftarget)(void) = (void (*)(void))(*(uint32_t *)(address + 4));
  __set_MSP(*(uint32_t *)address);
  pftarget();
  while(1);
}

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  // delay_init(72);

  //PA15 PB3 PB4 use gpio
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

#ifdef  LOG_DEBUG_ENABLE
  ulog_init();
#endif

  // stmencrypt_init();

  board_init();

  norflash_init();

  // fs_init();

  bool key_press = false;
  //按键检测
  if(!HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN))
  {
    HAL_Delay(20);
    if(!HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN))
    {
      key_press = true;
    }
  }

  //未按按键上电
  if(!key_press)
  {
    if(firmware_upgrade(APP_FW_START_ADDR))
    {
      jump_to_app(APP_FW_START_ADDR);
    }
  }

  //按住按键启动或者固件更新失败，启动u盘
  usbd_port_config(0);
  HAL_Delay(500);
  usbd_port_config(1);
  HAL_Delay(500);
  USBD_Init(&USBD_Device, &MSC_Desc, 0);
  USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
  USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);
  USBD_Start(&USBD_Device);

  while (1)
  {
    HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay(90);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = {0};

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;

  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;

  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /* USB clock selection */
  rccperiphclkinit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  rccperiphclkinit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  HAL_RCCEx_PeriphCLKConfig(&rccperiphclkinit);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
