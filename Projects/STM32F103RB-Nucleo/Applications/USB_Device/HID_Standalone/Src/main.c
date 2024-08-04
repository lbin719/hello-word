/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Src/main.c
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
#include "fs.h"
#include "stmflash.h"
#include "lcd.h"
#include "fonts.h"
#include "version.h"
#include "st7735s.h"
#include "text.h"
#include "stmencrypt.h"

const char CodeBuildDate[] = {__DATE__};
const char CodeBuildTime[] = {__TIME__};
/** @addtogroup STM32F1xx_HAL_Validation
  * @{
  */

/** @addtogroup STANDARD_CHECK
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CURSOR_STEP     5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;             /* USB Device澶勭悊缁撴瀯浣? */
extern volatile uint8_t g_usb_state_reg;    /* USB鐘舵�? */
extern volatile uint8_t g_device_state;     /* USB杩炴帴 鎯呭喌 */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  //PA15 PB3 PB4 use gpio
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  ulog_init();

  stm_encrypt_init();

  /* CmBacktrace initialize */
  cm_backtrace_init(PRODUCT_DEVICE_NAME, MCU_HW_VERSION, MCU_FW_VERSION);
  // cm_backtrace_set_callback(NULL);

  norflash_init();

  fs_init();

  fonts_init();

  lcd_init();

#if 0
  usbd_port_config(0);    /* USB鍏堟柇寮� */
  HAL_Delay(500);
  usbd_port_config(1);    /* USB鍐嶆?¤繛鎺? */
  HAL_Delay(500);
  USBD_Init(&USBD_Device, &MSC_Desc, 0);                              /* 鍒濆?嬪寲USB */
  USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);                   /* 娣诲姞绫? */
  USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);            /* 涓篗SC绫绘坊鍔犲洖璋冨嚱鏁? */
  USBD_Start(&USBD_Device);                                           /* 寮�鍚疷SB */
  HAL_Delay(1800);
#endif

  // text_show_string(0, 32, 160, 16, "翼飞冲天科技", 16, 0, RED);
    text_show_string(0, 0, 200, 16, "正点原子STM32开发板", 16, 0, RED);
    // text_show_string(0, 16, 200, 16, "GBK字库测试程序", 16, 0, RED);
    // text_show_string(30, 70, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
    // text_show_string(30, 90, 200, 16, "按KEY0,更新字库", 16, 0, RED);
    
    // text_show_string(30, 110, 200, 16, "内码高字节:", 16, 0, BLUE);
    // text_show_string(30, 130, 200, 16, "内码低字节:", 16, 0, BLUE);
    // text_show_string(30, 150, 200, 16, "汉字计数器:", 16, 0, BLUE);
    
    text_show_string(0, 32, 200, 24, "对应汉字为:", 24, 0, BLUE);
    // text_show_string(30, 204, 200, 16, "对应汉字(16*16)为:", 16, 0, BLUE);
    text_show_string(0, 64, 200, 16, "对应汉字(12*12)为:", 12, 0, BLUE);


  uint8_t test_buf[64];
  while (1)
  {
		snprintf(test_buf, sizeof(test_buf), "Tick: %ld", HAL_GetTick());
		text_show_string(0, 16, 160, 16, test_buf, 16, 0, RED);

    LOG_I("Hello world\r\n");
    /* Insert delay 100 ms */
    HAL_Delay(1000);
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
