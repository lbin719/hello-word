#include "main.h"
#include "ulog.h"
#include "usbd_storage.h"
#include "norflash.h"
#include "fs.h"
#include "stmflash.h"
#include "lcd.h"
#include "fonts.h"
#include "version.h"
#include "text.h"
#include "stmencrypt.h"
#include "hx711.h"
#include "mj8000.h"
#include "ec800e.h"
#include "wtn6040.h"


const char CodeBuildDate[] = {__DATE__};
const char CodeBuildTime[] = {__TIME__};

#if 0
extern USBD_StorageTypeDef  USBD_DISK_fops;
USBD_HandleTypeDef USBD_Device;

extern volatile uint8_t g_usb_state_reg;
extern volatile uint8_t g_device_state;
#endif

void SystemClock_Config(void);

void board_init(void)
{

}

extern void ui_init(void);
extern void ui_task_handle(void);

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock to 72 MHz */
  SystemClock_Config();

  delay_init(72);

  //PA15 PB3 PB4 use gpio
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

#ifdef LOG_DEBUG_ENABLE
  ulog_init();
  LOG_I("%s Demo\r\n", PRODUCT_DEVICE_NAME);
#endif

  stmencrypt_init();

  /* CmBacktrace initialize */
  cm_backtrace_init(PRODUCT_DEVICE_NAME, MCU_HW_VERSION, MCU_FW_VERSION);
  // cm_backtrace_set_callback(NULL);

  board_init();

  led_init();

  key_init();

  hx711_init();
  
  mj8000_init();

  ec800e_init();

  wtn6040_init();

  norflash_init();

#if 0
  usbd_port_config(0);
  HAL_Delay(500);
  usbd_port_config(1);
  HAL_Delay(500);
  USBD_Init(&USBD_Device, &MSC_Desc, 0);
  USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
  USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);
  USBD_Start(&USBD_Device);
  while(1);
  HAL_Delay(5000);
#endif

  fs_init();

  fonts_init();

  lcd_init();

  ui_init();

  while (1)
  {
#ifdef LOG_DEBUG_ENABLE
    usmart_scan();
#endif
    mj8000_task_handle();

    ec800e_task_handle();

    wtn6040_task_handle();

    hx711_task_handle();

    ui_task_handle();

    led_task_handle();
    // LOG_I("Hello world\r\n");
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
