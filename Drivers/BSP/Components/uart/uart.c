#include "uart.h"
#include "stm32f1xx_hal.h"
#include "main.h"


UART_HandleTypeDef Uart1Handle = {0};

void uart1_init(void)
{
  Uart1Handle.Instance        = USART1;
  Uart1Handle.Init.BaudRate   = 115200;
  Uart1Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart1Handle.Init.StopBits   = UART_STOPBITS_1;
  Uart1Handle.Init.Parity     = UART_PARITY_NONE;
  Uart1Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Uart1Handle.Init.Mode       = UART_MODE_TX_RX;
  if (HAL_UART_Init(&Uart1Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}

void uart1_sync_output(const uint8_t *pData, uint16_t Size)
{
   HAL_UART_Transmit(&Uart1Handle, (uint8_t *)pData, Size, 0xFFFF);

//  if (HAL_UART_Transmit_DMA(&Uart1Handle, (uint8_t *)pData, Size) != HAL_OK)
//  {
//    /* Transfer error in transmission process */
//    Error_Handler();
//  }
//  while(Uart1Handle.gState != HAL_UART_STATE_READY);
}



UART_HandleTypeDef Uart2Handle = {0};

void uart2_init(void)
{
  Uart2Handle.Instance        = USART2;
  Uart2Handle.Init.BaudRate   = 115200;
  Uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart2Handle.Init.StopBits   = UART_STOPBITS_1;
  Uart2Handle.Init.Parity     = UART_PARITY_NONE;
  Uart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Uart2Handle.Init.Mode       = UART_MODE_TX_RX;
  if (HAL_UART_Init(&Uart2Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}

void uart2_sync_output(const uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit(&Uart2Handle, (uint8_t *)pData, Size, 0xFFFF);

  // if (HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)pData, Size) != HAL_OK)
  // {
  //   /* Transfer error in transmission process */
  //   Error_Handler();
  // }
  // while(Uart2Handle.gState != HAL_UART_STATE_READY);
}


UART_HandleTypeDef Uart3Handle = {0};

void uart3_init(void)
{
  Uart3Handle.Instance        = USART3;
  Uart3Handle.Init.BaudRate   = 115200;
  Uart3Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart3Handle.Init.StopBits   = UART_STOPBITS_1;
  Uart3Handle.Init.Parity     = UART_PARITY_NONE;
  Uart3Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Uart3Handle.Init.Mode       = UART_MODE_TX;
  if (HAL_UART_Init(&Uart3Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}

void uart3_sync_output(const uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit(&Uart3Handle, (uint8_t *)pData, Size, 0xFFFF);
}





void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(huart->Instance == USART1)
  {
    // static DMA_HandleTypeDef hdma_tx;
    // static DMA_HandleTypeDef hdma_rx;

    __HAL_RCC_USART1_CLK_ENABLE();

    USART1_TX_GPIO_CLK_ENABLE();
    USART1_RX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = USART1_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART1_RX_GPIO_PIN;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

    // __HAL_RCC_DMA1_CLK_ENABLE();

    // /*##-3- Configure the DMA ##################################################*/
    // /* Configure the DMA handler for Transmission process */
    // hdma_tx.Instance                 = USART1_TX_DMA_CHANNEL;
    // hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    // hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    // hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    // hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    // hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    // hdma_tx.Init.Mode                = DMA_NORMAL;
    // hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    // HAL_DMA_Init(&hdma_tx);
    // /* Associate the initialized DMA handle to the UART handle */
    // __HAL_LINKDMA(huart, hdmatx, hdma_tx);

    // /* Configure the DMA handler for reception process */
    // hdma_rx.Instance                 = USART1_RX_DMA_CHANNEL;
    // hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    // hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    // hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    // hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    // hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    // hdma_rx.Init.Mode                = DMA_NORMAL;
    // hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    // HAL_DMA_Init(&hdma_rx);
    // /* Associate the initialized DMA handle to the the UART handle */
    // __HAL_LINKDMA(huart, hdmarx, hdma_rx);

    // HAL_NVIC_SetPriority(USART1_TX_DMA_IRQn, 0, 1);
    // HAL_NVIC_EnableIRQ(USART1_TX_DMA_IRQn);

    // HAL_NVIC_SetPriority(USART1_RX_DMA_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(USART1_RX_DMA_IRQn);

    // HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    // HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE();

    // __HAL_RCC_GPIOA_CLK_ENABLE();
    // /**USART1 GPIO Configuration
    // PA9      ------> USART1_TX
    // PA10     ------> USART1_RX
    // */
    // GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // GPIO_InitStruct.Pull = GPIO_PULLUP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();
    HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_GPIO_PIN);
    HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_GPIO_PIN);
  }
  else if(huart->Instance == USART2)
  {

  }

}

