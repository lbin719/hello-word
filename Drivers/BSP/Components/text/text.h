/**
 ****************************************************************************************************
 * @file        text.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-28
 * @brief       汉字显示 代码
 *              提供text_show_font和text_show_string两个函数,用于显示汉字
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200428
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __TEXT_H
#define __TEXT_H

#include "stdint.h"
#include "fonts.h"

#define USE_GBK2312_FONT


/* 函数声明 */
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint32_t color);
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint32_t color);
void text_show_string_middle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint32_t color);
void text_show_string_left(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint32_t color);
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint32_t color);

// void text_show_font_index(uint16_t x, uint16_t y, uint8_t index, uint8_t size, uint8_t mode, uint32_t color);

#endif
