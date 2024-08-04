/**
  ******************************************************************************
  * @file    USB_Device/HID_Standalone/Inc/usbd_conf.h
  * @author  MCD Application Team
  * @brief   General low level driver configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Common Config */
#define USBD_MAX_NUM_INTERFACES               1
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0x100
#define USBD_SUPPORT_USER_STRING_DESC         0
#define USBD_SELF_POWERED                     1
#define USBD_DEBUG_LEVEL                      0

/* MSC Class Config */
#define MSC_MEDIA_PACKET                      512   /* 定义包大小为2KB,越大,速度越快 */

/* 最大支持的设�?�?,3�? */
#define STORAGE_LUN_NBR                       1         /* 设置最大支持的设�?�? */

#define MAX_STATIC_ALLOC_SIZE                 1*1024 /* HID Class Driver Structure size */
/* Exported macro ------------------------------------------------------------*/
/* Memory management macros */
void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#define USBD_malloc                 (uint32_t *)USBD_static_malloc
#define USBD_free                   USBD_static_free
#define USBD_memset                 memset
#define USBD_memcpy                 memcpy
#define USBD_Delay                  HAL_Delay

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif

/* Exported functions ------------------------------------------------------- */
void usbd_port_config(uint8_t state);

#endif /* __USBD_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
