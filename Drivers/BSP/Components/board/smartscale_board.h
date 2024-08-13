/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMARTSCALE_BOARD_H
#define __SMARTSCALE_BOARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#define USART1_USE_DMA                  (0)

/* usb PA11 PA12*/
/* jlink PA13 PA14 */

/* uart1 TX:PA9 RX:PA10 */
#define USART1_TX_GPIO_PORT               GPIOA
#define USART1_TX_GPIO_PIN                GPIO_PIN_9
#define USART1_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1_RX_GPIO_PORT               GPIOA
#define USART1_RX_GPIO_PIN                GPIO_PIN_10
#define USART1_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#if USART1_USE_DMA
// #define USART1_TX_DMA_CHANNEL             DMA1_Channel4 //与spi2冲突
// #define USART1_RX_DMA_CHANNEL             DMA1_Channel5

// #define USART1_TX_DMA_IRQn                DMA1_Channel4_IRQn
// #define USART1_RX_DMA_IRQn                DMA1_Channel5_IRQn
// #define USART1_DMA_TX_IRQHandler          DMA1_Channel4_IRQHandler
// #define USART1_DMA_RX_IRQHandler          DMA1_Channel5_IRQHandler
#endif

/* uart2 TX:PA2 RX:PA3 */
// RX DMA1_Channel6
// TX DMA1_Channel7
#define USART2_TX_GPIO_PORT               GPIOA
#define USART2_TX_GPIO_PIN                GPIO_PIN_2
#define USART2_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_RX_GPIO_PORT               GPIOA
#define USART2_RX_GPIO_PIN                GPIO_PIN_3
#define USART2_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_TX_DMA_CHANNEL             DMA1_Channel7
#define USART2_RX_DMA_CHANNEL             DMA1_Channel6

#define USART2_TX_DMA_IRQn                DMA1_Channel7_IRQn
#define USART2_RX_DMA_IRQn                DMA1_Channel6_IRQn


/* uart3 TX:PB10 RX:PB11 */
#define USART3_TX_GPIO_PORT               GPIOB
#define USART3_TX_GPIO_PIN                GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define USART3_RX_GPIO_PORT               GPIOB
#define USART3_RX_GPIO_PIN                GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()


/* uart4 TX:PC10 RX:PC11 */
#define UART4_TX_GPIO_PORT                GPIOC
#define UART4_TX_GPIO_PIN                 GPIO_PIN_10
#define UART4_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

#define UART4_RX_GPIO_PORT                GPIOC
#define UART4_RX_GPIO_PIN                 GPIO_PIN_11
#define UART4_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

// TX DMA2_Channel5
// RX DMA2_Channel3
#define UART4_TX_DMA_CHANNEL              DMA2_Channel5
#define UART4_RX_DMA_CHANNEL              DMA2_Channel3

#define UART4_TX_DMA_IRQn                 DMA2_Channel4_5_IRQn
#define UART4_RX_DMA_IRQn                 DMA2_Channel3_IRQn
#define UART4_DMA_TX_IRQHandler           DMA2_Channel4_5_IRQHandler
#define UART4_DMA_RX_IRQHandler           DMA2_Channel3_IRQHandler

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

#define SPI3_SPI_CLK_ENABLE()               __HAL_RCC_SPI3_CLK_ENABLE()

#define SPI3_SCK_GPIO_PORT                  GPIOB
#define SPI3_SCK_GPIO_PIN                   GPIO_PIN_3
#define SPI3_SCK_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()

// #define SPI3_MISO_GPIO_PORT             GPIOB
// #define SPI3_MISO_GPIO_PIN              GPIO_PIN_4
// #define SPI3_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI3_MOSI_GPIO_PORT                 GPIOB
#define SPI3_MOSI_GPIO_PIN                  GPIO_PIN_5
#define SPI3_MOSI_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI3_TX_DMA_CHANNEL                 DMA2_Channel2
#define SPI3_RX_DMA_CHANNEL                 DMA2_Channel1
#define SPI3_DMA_CLK_ENABLE()               __HAL_RCC_DMA2_CLK_ENABLE()

#define SPI3_TX_DMA_IRQn                    DMA2_Channel2_IRQn
#define SPI3_RX_DMA_IRQn                    DMA2_Channel1_IRQn

#define LCD_CS_GPIO_PORT                    GPIOB
#define LCD_CS_GPIO_PIN                     GPIO_PIN_4
#define LCD_CS_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()


#define LCD_DC_GPIO_PORT                    GPIOB
#define LCD_DC_GPIO_PIN                     GPIO_PIN_6
#define LCD_DC_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()

#define LCD_RST_GPIO_PORT                   GPIOB
#define LCD_RST_GPIO_PIN                    GPIO_PIN_7
#define LCD_RST_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

#define LCD_BLK_GPIO_PORT                   GPIOD
#define LCD_BLK_GPIO_PIN                    GPIO_PIN_2
#define LCD_BLK_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOD_CLK_ENABLE()



//LED & KEY
#define LED_GREEN_GPIO_PORT                 GPIOC
#define LED_GREEN_GPIO_PIN                  GPIO_PIN_0
#define LED_GREEN_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

#define LED_BLUE_GPIO_PORT                  GPIOC
#define LED_BLUE_GPIO_PIN                   GPIO_PIN_1
#define LED_BLUE_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()

#define KEY_GPIO_PORT                       GPIOC
#define KEY_GPIO_PIN                        GPIO_PIN_2
#define KEY_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()

//HX711
#define HX711_SCK_GPIO_PORT                 GPIOC
#define HX711_SCK_GPIO_PIN                  GPIO_PIN_6
#define HX711_SCK_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()

#define HX711_DOUT_GPIO_PORT                GPIOC
#define HX711_DOUT_GPIO_PIN                 GPIO_PIN_7
#define HX711_DOUT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()

//WTN6040
#define WTN6040_DATA_GPIO_PORT              GPIOC
#define WTN6040_DATA_GPIO_PIN               GPIO_PIN_8
#define WTN6040_DATA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define WTN6040_BUSY_GPIO_PORT              GPIOC
#define WTN6040_BUSY_GPIO_PIN               GPIO_PIN_9
#define WTN6040_BUSY_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

/* uart1 log  PA9 PA10 */
// DMA1_Channel4
// DMA1_Channel5

/* uart2 4g  */
// DMA1_Channel6
// DMA1_Channel7

/* uart3 wifi */

/* uart4 qrcode */
// DMA2_Channel3
// DMA2_Channel5

/* spi1 norflash */
// DMA1_Channel3
// DMA1_Channel2

/* spi2 lcd */

/* spi3 lcd */
// DMA2_Channel1
// DMA2_Channel2





#ifdef __cplusplus
}
#endif

#endif /* __SMARTSCALE_BOARD_H */
