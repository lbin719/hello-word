/**
 ****************************************************************************************************
 * @file        fonts.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       字库 代码
 *              提供fonts_update_font和fonts_init用于字库更新和初始化
 * @license     Copyright (c) 2020-2032, 广州市星翼电子�?技有限�?��
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视�?:www.yuanzige.com
 * 技�??�?:www.openedv.com
 * �?��网址:www.alientek.com
 * �?��地址:openedv.taobao.com
 *
 * �?��说明
 * V1.0 20200428
 * �?��次发�?
 *
 ****************************************************************************************************
 */

#ifndef __FONTS_H
#define __FONTS_H

#include "stdint.h"
#include "stdbool.h"


/* 字体信息保存首地址
 * �?33�?���?,�?1�?��节用于标记字库是否存�?.后续�?8�?��节一�?,分别保存起�?地址和文件大�?
 */
extern uint32_t FONTINFOADDR;

/* 字库信息结构体定�?
 * 用来保存字库基本信息，地址，大小等
 */
// typedef __PACKED_STRUCT
typedef struct __attribute__((packed))
{
    uint8_t fontok;             /* 字库存在标志�?0XAA，字库�?常；其他，字库不存在 */
    uint32_t ugbkaddr;          /* unigbk的地址 */
    uint32_t ugbksize;          /* unigbk的大�? */
    uint32_t f12addr;           /* gbk12地址 */
    uint32_t gbk12size;         /* gbk12的大�? */
    uint32_t f16addr;           /* gbk16地址 */
    uint32_t gbk16size;         /* gbk16的大�? */
    uint32_t f24addr;           /* gbk24地址 */
    uint32_t gbk24size;         /* gbk24的大�? */
} _font_info;


/* 字库信息结构�? */
extern _font_info ftinfo;

/* 函数声明 */
bool fonts_init(void);                                                                                       /* 初�?化字�? */

#endif





















