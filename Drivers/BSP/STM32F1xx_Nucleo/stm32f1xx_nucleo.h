/**
  ******************************************************************************
  * @file    stm32f1xx_nucleo.h
  * @author  MCD Application Team
  * @version V1.0.4
  * @date    14-April-2017
  * @brief   This file contains definitions for:
  *          - LEDs and push-button available on STM32F1XX-Nucleo Kit
  *            from STMicroelectronics
  *          - LCD, joystick and microSD available on Adafruit 1.8" TFT LCD
  *            shield (reference ID 802)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1XX_NUCLEO_H
#define __STM32F1XX_NUCLEO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* jlink PA13 PA14 */
/* usb PA11 PA12*/

/* uart1 TX:PA9 RX:PA10 */
#define USART1_TX_GPIO_PORT               GPIOA
#define USART1_TX_GPIO_PIN                GPIO_PIN_9
#define USART1_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1_RX_GPIO_PORT               GPIOA
#define USART1_RX_GPIO_PIN                GPIO_PIN_10
#define USART1_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

// #define USART1_TX_DMA_CHANNEL             DMA1_Channel4 //与spi2冲突
// #define USART1_RX_DMA_CHANNEL             DMA1_Channel5

// #define USART1_TX_DMA_IRQn                DMA1_Channel4_IRQn
// #define USART1_RX_DMA_IRQn                DMA1_Channel5_IRQn
// #define USART1_DMA_TX_IRQHandler          DMA1_Channel4_IRQHandler
// #define USART1_DMA_RX_IRQHandler          DMA1_Channel5_IRQHandler


/* uart2 TX:PA2 RX:PA3 */
#define USART2_TX_GPIO_PORT               GPIOA
#define USART2_TX_GPIO_PIN                GPIO_PIN_2
#define USART2_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_RX_GPIO_PORT               GPIOA
#define USART2_RX_GPIO_PIN                GPIO_PIN_3
#define USART2_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_TX_DMA_CHANNEL             DMA1_Channel6
#define USART2_RX_DMA_CHANNEL             DMA1_Channel7

#define USART2_TX_DMA_IRQn                DMA1_Channel6_IRQn
#define USART2_RX_DMA_IRQn                DMA1_Channel7_IRQn

/* uart3 TX:PB10 RX:PB11 */
#define USART3_TX_GPIO_PORT               GPIOB
#define USART3_TX_GPIO_PIN                GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define USART3_RX_GPIO_PORT               GPIOB
#define USART3_RX_GPIO_PIN                GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()


/* spi1 */
// #define SPI1_SPI                        SPI1
#define SPI1_SPI_CLK_ENABLE()           __HAL_RCC_SPI1_CLK_ENABLE()

#define SPI1_SCK_GPIO_PORT              GPIOA
#define SPI1_SCK_GPIO_PIN               GPIO_PIN_5
#define SPI1_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPI1_MISO_GPIO_PORT             GPIOA
#define SPI1_MISO_GPIO_PIN              GPIO_PIN_6
#define SPI1_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPI1_MOSI_GPIO_PORT             GPIOA
#define SPI1_MOSI_GPIO_PIN              GPIO_PIN_7
#define SPI1_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()


#define SPI1_TX_DMA_CHANNEL              DMA1_Channel3
#define SPI1_RX_DMA_CHANNEL              DMA1_Channel2
#define SPI1_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()

#define SPI1_TX_DMA_IRQn                 DMA1_Channel3_IRQn
#define SPI1_RX_DMA_IRQn                 DMA1_Channel2_IRQn


/* spi2 */
// #define SPI2_SPI                        SPI2
#define SPI2_SPI_CLK_ENABLE()           __HAL_RCC_SPI2_CLK_ENABLE()

#define SPI2_SCK_GPIO_PORT              GPIOB
#define SPI2_SCK_GPIO_PIN               GPIO_PIN_13
#define SPI2_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI2_MISO_GPIO_PORT             GPIOB
#define SPI2_MISO_GPIO_PIN              GPIO_PIN_14
#define SPI2_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI2_MOSI_GPIO_PORT             GPIOB
#define SPI2_MOSI_GPIO_PIN              GPIO_PIN_15
#define SPI2_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()


#define SPI2_TX_DMA_CHANNEL              DMA1_Channel5
#define SPI2_RX_DMA_CHANNEL              DMA1_Channel4
#define SPI2_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()

#define SPI2_TX_DMA_IRQn                 DMA1_Channel5_IRQn
#define SPI2_RX_DMA_IRQn                 DMA1_Channel4_IRQn


#define NORFLASH_CS_GPIO_PORT           GPIOB
#define NORFLASH_CS_GPIO_PIN            GPIO_PIN_12
#define NORFLASH_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()



/* spi3 */
// #define SPI3_SPI                        SPI3
#define SPI3_SPI_CLK_ENABLE()           __HAL_RCC_SPI3_CLK_ENABLE()

#define SPI3_SCK_GPIO_PORT              GPIOB
#define SPI3_SCK_GPIO_PIN               GPIO_PIN_3
#define SPI3_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

// #define SPI3_MISO_GPIO_PORT             GPIOB
// #define SPI3_MISO_GPIO_PIN              GPIO_PIN_4
// #define SPI3_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI3_MOSI_GPIO_PORT             GPIOB
#define SPI3_MOSI_GPIO_PIN              GPIO_PIN_5
#define SPI3_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI3_TX_DMA_CHANNEL              DMA2_Channel2
#define SPI3_RX_DMA_CHANNEL              DMA2_Channel1
#define SPI3_DMA_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()

#define SPI3_TX_DMA_IRQn                 DMA2_Channel2_IRQn
#define SPI3_RX_DMA_IRQn                 DMA2_Channel1_IRQn

#define LCD_CS_GPIO_PORT                 GPIOB
#define LCD_CS_GPIO_PIN                  GPIO_PIN_4
#define LCD_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()


#define LCD_DC_GPIO_PORT                 GPIOB
#define LCD_DC_GPIO_PIN                  GPIO_PIN_6
#define LCD_DC_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

#define LCD_RST_GPIO_PORT                 GPIOB
#define LCD_RST_GPIO_PIN                  GPIO_PIN_7
#define LCD_RST_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

/* uart1 log  PA9 PA10 */
// DMA1_Channel4
// DMA1_Channel5

/* uart2 4g  */
// DMA1_Channel6
// DMA1_Channel7
/* uart3 wifi */

/* uart4  */
// DMA2_Channel3
// DMA2_Channel5

/* spi1 norflash */
// DMA1_Channel3
// DMA1_Channel2

/* spi2 lcd */

/* spi3 lcd */
// DMA2_Channel1
// DMA2_Channel2

/* i2c 12bit adc ic */
/* i2c yuyin mode */

/* qrcode */





#ifdef __cplusplus
}
#endif

#endif /* __STM32F1XX_NUCLEO_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
