#include "uart.h"
#include "stm32f1xx_hal.h"
#include "main.h"


#define RXBUFFERSIZE   1                        /* 缓存大小 */

uint8_t uart1_rx_buffer[RXBUFFERSIZE];              /* HAL库使用的串口接收缓冲 */
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
    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)uart1_rx_buffer, RXBUFFERSIZE);
}

void uart1_sync_output(uint8_t *pData, uint16_t Size)
{
#if USART1_USE_DMA
  if (HAL_UART_Transmit_DMA(&Uart1Handle, pData, Size) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
  while(Uart1Handle.gState != HAL_UART_STATE_READY);
#else
  HAL_UART_Transmit(&Uart1Handle, pData, Size, HAL_MAX_DELAY);
#endif

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
  // HAL_UART_Transmit(&Uart2Handle, (uint8_t *)pData, Size, HAL_MAX_DELAY);

  if (HAL_UART_Transmit_DMA(&Uart2Handle, (uint8_t *)pData, Size) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }
  while(Uart2Handle.gState != HAL_UART_STATE_READY);
}

void uart2_recive_dma(uint8_t *pData, uint16_t Size)
{
  HAL_UART_Receive_DMA(&Uart2Handle, pData, Size);
}

#if 0
UART_HandleTypeDef Uart3Handle = {0};

void uart3_init(void)
{
  Uart3Handle.Instance        = USART3;
  Uart3Handle.Init.BaudRate   = 115200;
  Uart3Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart3Handle.Init.StopBits   = UART_STOPBITS_1;
  Uart3Handle.Init.Parity     = UART_PARITY_NONE;
  Uart3Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Uart3Handle.Init.Mode       = UART_MODE_TX_RX;
  if (HAL_UART_Init(&Uart3Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}

void uart3_sync_output(const uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit(&Uart3Handle, (uint8_t *)pData, Size, HAL_MAX_DELAY);
}
#endif


UART_HandleTypeDef Uart4Handle = {0};

void uart4_init(void)
{
  Uart4Handle.Instance        = UART4;
  Uart4Handle.Init.BaudRate   = 9600;
  Uart4Handle.Init.WordLength = UART_WORDLENGTH_8B;
  Uart4Handle.Init.StopBits   = UART_STOPBITS_1;
  Uart4Handle.Init.Parity     = UART_PARITY_NONE;
  Uart4Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  Uart4Handle.Init.Mode       = UART_MODE_TX_RX;
  if (HAL_UART_Init(&Uart4Handle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}

void uart4_sync_output(uint8_t *pData, uint16_t Size)
{
  HAL_UART_Transmit(&Uart4Handle, (uint8_t *)pData, Size, HAL_MAX_DELAY);
}

void uart4_recive_dma(uint8_t *pData, uint16_t Size)
{
  HAL_UART_Receive_DMA(&Uart4Handle, pData, Size);
}


/**
 * @brief       串口数据接收回调函数
                数据处理在这里进行
 * @param       huart:串口句柄
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)                    /* 如果是串口1 */
  {
  #ifdef LOG_DEBUG_ENABLE
    extern void usmart_recive_callback(uint8_t *data, uint16_t len);
    usmart_recive_callback(uart1_rx_buffer, 1);
  #endif
    HAL_UART_Receive_IT(huart, (uint8_t *)uart1_rx_buffer, RXBUFFERSIZE);
  }
}


#if USART1_USE_DMA
static DMA_HandleTypeDef hdma_uart1tx;
static DMA_HandleTypeDef hdma_uart1rx;
#endif

static DMA_HandleTypeDef hdma_uart2tx;
static DMA_HandleTypeDef hdma_uart2rx;

static DMA_HandleTypeDef hdma_uart4tx;
static DMA_HandleTypeDef hdma_uart4rx;

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(huart->Instance == USART1)
  {
    __HAL_RCC_USART1_CLK_ENABLE();

    USART1_TX_GPIO_CLK_ENABLE();
    USART1_RX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = USART1_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART1_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

#if USART1_USE_DMA
    __HAL_RCC_DMA1_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_uart1tx.Instance                 = USART1_TX_DMA_CHANNEL;
    hdma_uart1tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_uart1tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart1tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart1tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart1tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart1tx.Init.Mode                = DMA_NORMAL;
    hdma_uart1tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_uart1tx);
    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_uart1tx);

    /* Configure the DMA handler for reception process */
    hdma_uart1rx.Instance                 = USART1_RX_DMA_CHANNEL;
    hdma_uart1rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_uart1rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart1rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart1rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart1rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart1rx.Init.Mode                = DMA_NORMAL;
    hdma_uart1rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_uart1rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_uart1rx);

    HAL_NVIC_SetPriority(USART1_TX_DMA_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_TX_DMA_IRQn);

    HAL_NVIC_SetPriority(USART1_RX_DMA_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_RX_DMA_IRQn);
#endif

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

  }
  else if(huart->Instance == USART2)
  {
    __HAL_RCC_USART2_CLK_ENABLE();

    USART2_TX_GPIO_CLK_ENABLE();
    USART2_RX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = USART2_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART2_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

    __HAL_RCC_DMA1_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_uart2tx.Instance                 = USART2_TX_DMA_CHANNEL;
    hdma_uart2tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_uart2tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart2tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart2tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart2tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart2tx.Init.Mode                = DMA_NORMAL;
    hdma_uart2tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_uart2tx);
    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_uart2tx);

    /* Configure the DMA handler for reception process */
    hdma_uart2rx.Instance                 = USART2_RX_DMA_CHANNEL;
    hdma_uart2rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_uart2rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart2rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart2rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart2rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart2rx.Init.Mode                = DMA_NORMAL;
    hdma_uart2rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_uart2rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_uart2rx);

    HAL_NVIC_SetPriority(USART2_TX_DMA_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(USART2_TX_DMA_IRQn);

    HAL_NVIC_SetPriority(USART2_RX_DMA_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_RX_DMA_IRQn);

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  }
  else if(huart->Instance == UART4)
  {
    __HAL_RCC_UART4_CLK_ENABLE();

    UART4_TX_GPIO_CLK_ENABLE();
    UART4_RX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = UART4_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(UART4_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = UART4_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(UART4_RX_GPIO_PORT, &GPIO_InitStruct);

    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_uart4tx.Instance                 = UART4_TX_DMA_CHANNEL;
    hdma_uart4tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_uart4tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart4tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart4tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart4tx.Init.Mode                = DMA_NORMAL;
    hdma_uart4tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_uart4tx);
    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA(huart, hdmatx, hdma_uart4tx);

    /* Configure the DMA handler for reception process */
    hdma_uart4rx.Instance                 = UART4_RX_DMA_CHANNEL;
    hdma_uart4rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_uart4rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart4rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart4rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart4rx.Init.Mode                = DMA_NORMAL;
    hdma_uart4rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_uart4rx);
    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_uart4rx);

    HAL_NVIC_SetPriority(UART4_TX_DMA_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART4_TX_DMA_IRQn);

    HAL_NVIC_SetPriority(UART4_RX_DMA_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART4_RX_DMA_IRQn);

    HAL_NVIC_SetPriority(UART4_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance == USART1)
  {
    // __HAL_RCC_USART1_CLK_DISABLE();
    // HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_GPIO_PIN);
    // HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_GPIO_PIN);
  }
  else if(huart->Instance == USART2)
  {

  }

}

