#ifndef _SPI_H
#define _SPI_H

#include "stm32f1xx.h"
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define SPI_FLASH_TIMEOUT   100

extern SPI_HandleTypeDef Spi1Handle; /* SPI1句柄 */

void spi1_init(void);

#define spi1_bytes_write(pbuffer, length)  HAL_SPI_Transmit(&Spi1Handle, pbuffer, length, SPI_FLASH_TIMEOUT)
#define spi1_bytes_read(pbuffer, length)   HAL_SPI_Receive(&Spi1Handle, pbuffer, length, SPI_FLASH_TIMEOUT)

#define spi1_dma_write(pbuffer, length)    HAL_SPI_Transmit_DMA(&Spi1Handle, pbuffer, length)
#define spi1_dma_read(pbuffer, length)     HAL_SPI_Receive_DMA(&Spi1Handle, pbuffer, length)
#define spi1_dma_wait_finsh()              while(HAL_SPI_GetState(&Spi1Handle) != HAL_SPI_STATE_READY){}


extern SPI_HandleTypeDef Spi2Handle; /* SPI2句柄 */

void spi2_init(void);

#define spi2_bytes_write(pbuffer, length)  HAL_SPI_Transmit(&Spi2Handle, pbuffer, length, SPI_FLASH_TIMEOUT)
#define spi2_bytes_read(pbuffer, length)   HAL_SPI_Receive(&Spi2Handle, pbuffer, length, SPI_FLASH_TIMEOUT)

#define spi2_dma_write(pbuffer, length)    HAL_SPI_Transmit_DMA(&Spi2Handle, pbuffer, length)
#define spi2_dma_read(pbuffer, length)     HAL_SPI_Receive_DMA(&Spi2Handle, pbuffer, length)
#define spi2_dma_wait_finsh()              while(HAL_SPI_GetState(&Spi2Handle) != HAL_SPI_STATE_READY){}

#ifdef __cplusplus
 }
#endif

#endif /* _SPI_H */