/**
  ******************************************************************************
  * @file    HID_Standalone/Src/stm32f1xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"

/** @addtogroup Validation_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd;

extern UART_HandleTypeDef Uart1Handle;
extern UART_HandleTypeDef Uart2Handle;
extern UART_HandleTypeDef Uart4Handle;

extern SPI_HandleTypeDef Spi1Handle; /* SPI1句柄 */
extern SPI_HandleTypeDef Spi2Handle; /* SPI2句柄 */
extern SPI_HandleTypeDef Spi3Handle; /* SPI3句柄 */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
// void HardFault_Handler(void)
// {
//   /* Go to infinite loop when Hard Fault exception occurs */
//   while (1)
//   {
//   }
// }

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
// void SVC_Handler(void)
// {
// }

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
// void SysTick_Handler(void)
// {
//   HAL_IncTick();
// }

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f1xx.s).                                               */
/******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}

void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi1Handle.hdmarx);
}

void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi1Handle.hdmatx);
}

void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi2Handle.hdmarx);
}

void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi2Handle.hdmatx);
}


void DMA2_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi3Handle.hdmarx);
}

void DMA2_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Spi3Handle.hdmatx);
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&Uart1Handle);
}


void DMA1_Channel6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart2Handle.hdmarx);
}

void DMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart2Handle.hdmatx);
}

void USART2_IRQHandler(void)
{
  if(__HAL_UART_GET_FLAG(&Uart2Handle, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&Uart2Handle);

    HAL_UART_AbortReceive(&Uart2Handle);

    extern void ec800e_uart_rx_callback(UART_HandleTypeDef *huart);
    ec800e_uart_rx_callback(&Uart2Handle);
  }

  HAL_UART_IRQHandler(&Uart2Handle);
}

void DMA2_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart4Handle.hdmarx);
}

void DMA2_Channel4_5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart4Handle.hdmatx);
}


void UART4_IRQHandler(void)
{
  if(__HAL_UART_GET_FLAG(&Uart4Handle, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&Uart4Handle);

    HAL_UART_AbortReceive(&Uart4Handle);

    extern void mj8000_uart_rx_callback(UART_HandleTypeDef *huart);
    mj8000_uart_rx_callback(&Uart4Handle);
  }
  HAL_UART_IRQHandler(&Uart4Handle);
}

/**
  * @brief  This function handles external lines interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
//  HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
