#include "spi.h"
#include "stm32f1xx_hal.h"
#include "main.h"


SPI_HandleTypeDef Spi1Handle; /* SPI1句柄 */

void spi1_init(void)
{
    Spi1Handle.Instance = SPI1;
    Spi1Handle.Init.Mode = SPI_MODE_MASTER;                        /* 设置SPI工作模式，设置为主模式 */
    Spi1Handle.Init.Direction = SPI_DIRECTION_1LINE;              
    Spi1Handle.Init.DataSize = SPI_DATASIZE_8BIT;                  /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    Spi1Handle.Init.CLKPolarity = SPI_POLARITY_LOW;                /* 串行同步时钟的空闲状态 */
    Spi1Handle.Init.CLKPhase = SPI_PHASE_1EDGE;                    /* 串行同步时钟的第几个跳变沿（上升或下降）数据被采样 */
    Spi1Handle.Init.NSS = SPI_NSS_SOFT;                            /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;  /* 定义波特率预分频的值:波特率预分频值为 */
    Spi1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    Spi1Handle.Init.TIMode = SPI_TIMODE_DISABLE;                   /* 关闭TI模式 */
    Spi1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    Spi1Handle.Init.CRCPolynomial = 7;                             /* CRC值计算的多项式 */

    if (HAL_SPI_Init(&Spi1Handle) != HAL_OK)
    {
        Error_Handler();
    }
    // __HAL_SPI_ENABLE(&Spi1Handle);
}


SPI_HandleTypeDef Spi2Handle; /* SPI1句柄 */

void spi2_init(void)
{
    Spi2Handle.Instance = SPI2;
    Spi2Handle.Init.Mode = SPI_MODE_MASTER;                        /* 设置SPI工作模式，设置为主模式 */
    Spi2Handle.Init.Direction = SPI_DIRECTION_2LINES;              /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    Spi2Handle.Init.DataSize = SPI_DATASIZE_8BIT;                  /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    Spi2Handle.Init.CLKPolarity = SPI_POLARITY_HIGH;               /* 串行同步时钟的空闲状态为高电平 */
    Spi2Handle.Init.CLKPhase = SPI_PHASE_2EDGE;                    /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
    Spi2Handle.Init.NSS = SPI_NSS_SOFT;                            /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    Spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;  /* 定义波特率预分频的值:波特率预分频值为 */
    Spi2Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    Spi2Handle.Init.TIMode = SPI_TIMODE_DISABLE;                   /* 关闭TI模式 */
    Spi2Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    Spi2Handle.Init.CRCPolynomial = 7;                             /* CRC值计算的多项式 */

    if (HAL_SPI_Init(&Spi2Handle) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_SPI_ENABLE(&Spi2Handle);
}


SPI_HandleTypeDef Spi3Handle; /* SPI3句柄 */

void spi3_init(void)
{
    Spi3Handle.Instance = SPI3;
    Spi3Handle.Init.Mode = SPI_MODE_MASTER;                        /* 设置SPI工作模式，设置为主模式 */
    Spi3Handle.Init.Direction = SPI_DIRECTION_1LINE; // SPI_DIRECTION_2LINES;
    Spi3Handle.Init.DataSize = SPI_DATASIZE_8BIT;                  /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    Spi3Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    Spi3Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    Spi3Handle.Init.NSS = SPI_NSS_SOFT;                            /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    Spi3Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  /* 定义波特率预分频的值:波特率预分频值为 */
    Spi3Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    Spi3Handle.Init.TIMode = SPI_TIMODE_DISABLE;                   /* 关闭TI模式 */
    Spi3Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    Spi3Handle.Init.CRCPolynomial = 7;                             /* CRC值计算的多项式 */

    if (HAL_SPI_Init(&Spi3Handle) != HAL_OK)
    {
        Error_Handler();
    }
//    __HAL_SPI_ENABLE(&Spi3Handle);
}

static DMA_HandleTypeDef spi1hdma_tx;
static DMA_HandleTypeDef spi1hdma_rx;

static DMA_HandleTypeDef spi2hdma_tx;
static DMA_HandleTypeDef spi2hdma_rx;

static DMA_HandleTypeDef spi3hdma_tx;
static DMA_HandleTypeDef spi3hdma_rx;

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef gpio_init_struct;
  if (hspi->Instance == SPI1)
  {
    SPI1_SCK_GPIO_CLK_ENABLE();  /* SPI1_SCK脚时钟使能 */
//    SPI1_MISO_GPIO_CLK_ENABLE(); /* SPI1_MISO脚时钟使能 */
    SPI1_MOSI_GPIO_CLK_ENABLE(); /* SPI1_MOSI脚时钟使能 */

    /* SCK引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI1_SCK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &gpio_init_struct);

    /* MISO引脚模式设置(复用输出) */
    // gpio_init_struct.Pin = SPI1_MISO_GPIO_PIN;
    // HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &gpio_init_struct);

    /* MOSI引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI1_MOSI_GPIO_PIN;
    HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &gpio_init_struct);

    /* Enable SPI1 clock */
    SPI1_SPI_CLK_ENABLE();

    /* Enable DMA clock */
    SPI1_DMA_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    spi1hdma_tx.Instance                 = SPI1_TX_DMA_CHANNEL;
    spi1hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    spi1hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi1hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    spi1hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi1hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi1hdma_tx.Init.Mode                = DMA_NORMAL;
    spi1hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&spi1hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, spi1hdma_tx);

    /* Configure the DMA handler for Transmission process */
    spi1hdma_rx.Instance                 = SPI1_RX_DMA_CHANNEL;
    spi1hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    spi1hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi1hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    spi1hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi1hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi1hdma_rx.Init.Mode                = DMA_NORMAL;
    spi1hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&spi1hdma_rx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, spi1hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
    HAL_NVIC_SetPriority(SPI1_TX_DMA_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPI1_TX_DMA_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
    HAL_NVIC_SetPriority(SPI1_RX_DMA_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI1_RX_DMA_IRQn);
  }
  else if (hspi->Instance == SPI2)
  {
    SPI2_SCK_GPIO_CLK_ENABLE();  /* SPI2_SCK脚时钟使能 */
    SPI2_MISO_GPIO_CLK_ENABLE(); /* SPI2_MISO脚时钟使能 */
    SPI2_MOSI_GPIO_CLK_ENABLE(); /* SPI2_MOSI脚时钟使能 */

    /* SCK引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI2_SCK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &gpio_init_struct);

    /* MISO引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI2_MISO_GPIO_PIN;
    HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &gpio_init_struct);

    /* MOSI引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI2_MOSI_GPIO_PIN;
    HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &gpio_init_struct);

    /* Enable SPI2 clock */
    SPI2_SPI_CLK_ENABLE();

    // /* Enable DMA clock */
    SPI2_DMA_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    spi2hdma_tx.Instance                 = SPI2_TX_DMA_CHANNEL;
    spi2hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    spi2hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi2hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    spi2hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi2hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi2hdma_tx.Init.Mode                = DMA_NORMAL;
    spi2hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&spi2hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, spi2hdma_tx);

    /* Configure the DMA handler for Transmission process */
    spi2hdma_rx.Instance                 = SPI2_RX_DMA_CHANNEL;
    spi2hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    spi2hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi2hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    spi2hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi2hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi2hdma_rx.Init.Mode                = DMA_NORMAL;
    spi2hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&spi2hdma_rx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, spi2hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (SPI2_TX) */
    HAL_NVIC_SetPriority(SPI2_TX_DMA_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPI2_TX_DMA_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (SPI2_RX) */
    HAL_NVIC_SetPriority(SPI2_RX_DMA_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI2_RX_DMA_IRQn);
  }
  else if (hspi->Instance == SPI3)
  {
#if LCD_USE_SPI3
    SPI3_SCK_GPIO_CLK_ENABLE();  /* SPI3_SCK脚时钟使能 */
    // SPI3_MISO_GPIO_CLK_ENABLE(); /* SPI3_MISO脚时钟使能 */
    SPI3_MOSI_GPIO_CLK_ENABLE(); /* SPI3_MOSI脚时钟使能 */

    /* SCK引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI3_SCK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPI3_SCK_GPIO_PORT, &gpio_init_struct);

    /* MISO引脚模式设置(复用输出) */
    // gpio_init_struct.Pin = SPI3_MISO_GPIO_PIN;
    // HAL_GPIO_Init(SPI3_MISO_GPIO_PORT, &gpio_init_struct);

    /* MOSI引脚模式设置(复用输出) */
    gpio_init_struct.Pin = SPI3_MOSI_GPIO_PIN;
    HAL_GPIO_Init(SPI3_MOSI_GPIO_PORT, &gpio_init_struct);

    /* Enable SPI3 clock */
    SPI3_SPI_CLK_ENABLE();

    // /* Enable DMA clock */
    SPI3_DMA_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    spi3hdma_tx.Instance                 = SPI3_TX_DMA_CHANNEL;
    spi3hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    spi3hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi3hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    spi3hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi3hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi3hdma_tx.Init.Mode                = DMA_NORMAL;
    spi3hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&spi3hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, spi3hdma_tx);

    /* Configure the DMA handler for Transmission process */
    spi3hdma_rx.Instance                 = SPI3_RX_DMA_CHANNEL;
    spi3hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    spi3hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    spi3hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    spi3hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    spi3hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    spi3hdma_rx.Init.Mode                = DMA_NORMAL;
    spi3hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&spi3hdma_rx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, spi3hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (SPI2_TX) */
    HAL_NVIC_SetPriority(SPI3_TX_DMA_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPI3_TX_DMA_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (SPI2_RX) */
    HAL_NVIC_SetPriority(SPI3_RX_DMA_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SPI3_RX_DMA_IRQn);
#endif
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI1)
  {
    /*##-1- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SPI1_SCK_GPIO_PORT, SPI1_SCK_GPIO_PIN);
    /* Configure SPI MISO as alternate function  */
    // HAL_GPIO_DeInit(SPI1_MISO_GPIO_PORT, SPI1_MISO_GPIO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_GPIO_PIN);

    /*##-2- Disable the DMA ####################################################*/
    /* De-Initialize the DMA associated to transmission process */
    HAL_DMA_DeInit(&spi1hdma_tx);
    /* De-Initialize the DMA associated to reception process */
    HAL_DMA_DeInit(&spi1hdma_rx);

    /*##-3- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(SPI1_TX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(SPI1_RX_DMA_IRQn);
  }
  else if (hspi->Instance == SPI2)
  {
    /*##-3- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(SPI2_TX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(SPI2_RX_DMA_IRQn);

    /*##-1- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SPI2_SCK_GPIO_PORT, SPI2_SCK_GPIO_PIN);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(SPI2_MISO_GPIO_PORT, SPI2_MISO_GPIO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SPI2_MOSI_GPIO_PORT, SPI2_MOSI_GPIO_PIN);

    // /*##-2- Disable the DMA ####################################################*/
    /* De-Initialize the DMA associated to transmission process */
    HAL_DMA_DeInit(&spi2hdma_tx);
    /* De-Initialize the DMA associated to reception process */
    HAL_DMA_DeInit(&spi2hdma_rx);

    /*##-3- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(SPI2_TX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(SPI2_RX_DMA_IRQn);
  }
  else if (hspi->Instance == SPI3)
  {
#if LCD_USE_SPI3
    /*##-1- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SPI3_SCK_GPIO_PORT, SPI3_SCK_GPIO_PIN);
    /* Configure SPI MISO as alternate function  */
    // HAL_GPIO_DeInit(SPI3_MISO_GPIO_PORT, SPI3_MISO_GPIO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SPI3_MOSI_GPIO_PORT, SPI3_MOSI_GPIO_PIN);

    // /*##-2- Disable the DMA ####################################################*/
    /* De-Initialize the DMA associated to transmission process */
    HAL_DMA_DeInit(&spi3hdma_tx);
    /* De-Initialize the DMA associated to reception process */
    HAL_DMA_DeInit(&spi3hdma_rx);

    /*##-3- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(SPI3_TX_DMA_IRQn);
    HAL_NVIC_DisableIRQ(SPI3_RX_DMA_IRQn);
#endif
  }
}

