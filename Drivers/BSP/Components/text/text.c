/**
 ****************************************************************************************************
 * @file        text.c
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

#include "string.h"
#include "text.h"
#include "ff.h"
#include "lcd.h"
#include "ulog.h"
#include "font.h"

/**
 * @brief       获取汉字点阵数据
 * @param       code  : 当前汉字编码(GBK码)
 * @param       mat   : 当前汉字点阵数据存放地址
 * @param       size  : 字体大小
 *   @note      size大小的字体,其点阵数据大小为: (size / 8 + ((size % 8) ? 1 : 0)) * (size)  字节
 * @retval      无
 */
static void text_get_hz_mat(unsigned char *code, unsigned char *mat, uint8_t size)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); /* 得到字体一个字符对应点阵集所占的字节数 */
    qh = *code;
    ql = *(++code);
#if 0
    LOG_I("text qh:%02x ql:%02x\r\n", qh, ql);
#endif
    if (qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff)     /* 非 常用汉字 */
    {
        for (i = 0; i < csize; i++)
        {
            *mat++ = 0x00;  /* 填充满格 */
        }

        return;     /* 结束访问 */
    }

    if (ql < 0x7f)
    {
        ql -= 0x40; /* 注意! */
    }
    else
    {
        ql -= 0x41;
    }

    qh -= 0x81;
    foffset = ((unsigned long)190 * qh + ql) * csize;   /* 得到字库中的字节偏移量 */

	FIL File;
    uint32_t readbytes;

    switch (size)
    {
        case 12:
            f_open(&File, "GBK12.FON", FA_READ);
            // norflash_read(mat, foffset + ftinfo.f12addr, csize);
            break;

        case 16:
            f_open(&File, "GBK16.FON", FA_READ);
            // norflash_read(mat, foffset + ftinfo.f16addr, csize);
            break;

        case 24:
            f_open(&File, "GBK24.FON", FA_READ);
            // norflash_read(mat, foffset + ftinfo.f24addr, csize);
            break;
    }

    f_lseek(&File, foffset);
    f_read(&File, mat, csize, (UINT *)&readbytes);
    f_close(&File);
}

/**
 * @brief       显示一个指定大小的汉字
 * @param       x,y   : 汉字的坐标
 * @param       font  : 汉字GBK码
 * @param       size  : 字体大小
 * @param       mode  : 显示模式
 *   @note              0, 正常显示(不需要显示的点,用LCD背景色填充,即g_back_color)
 *   @note              1, 叠加显示(仅显示需要显示的点, 不需要显示的点, 不做处理)
 * @param       color : 字体颜色
 * @retval      无
 */
#define DZK_LENGTH      255
static uint8_t dzk[DZK_LENGTH];
void text_show_font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t, t1;
    uint16_t y0 = y;
    // uint8_t *dzk;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size);     /* 得到字体一个字符对应点阵集所占的字节数 */

    if (size != 12 && size != 16 && size != 24 && size != 32)
    {
        return;     /* 不支持的size */
    }

    // dzk = mymalloc(SRAMIN, size);       /* 申请内存 */ TODO:应该是csize

    // if (dzk == 0) return;               /* 内存不够了 */
    if(csize > DZK_LENGTH)
    {
        return;
    }

    text_get_hz_mat(font, dzk, size);   /* 得到相应大小的点阵数据 */

     for (t = 0; t < csize; t++)
     {
         temp = dzk[t];                  /* 得到点阵数据 */

         for (t1 = 0; t1 < 8; t1++)
         {
             if (temp & 0x80)
             {
                 lcd_draw_point(x, y, color);        /* 画需要显示的点 */
             }
             else if (mode == 0)     /* 如果非叠加模式, 不需要显示的点,用背景色填充 */
             {
                 lcd_draw_point(x, y, g_back_color); /* 填充背景色 */
             }

             temp <<= 1;
             y++;

             if ((y - y0) == size)
             {
                 y = y0;
                 x++;
                 break;
             }
         }
     }

    // myfree(SRAMIN, dzk);    /* 释放内存 */
}

/**
 * @brief       在指定位置开始显示一个字符串
 *   @note      该函数支持自动换行
 * @param       x,y   : 起始坐标
 * @param       width : 显示区域宽度
 * @param       height: 显示区域高度
 * @param       str   : 字符串
 * @param       size  : 字体大小
 * @param       mode  : 显示模式
 *   @note              0, 正常显示(不需要显示的点,用LCD背景色填充,即g_back_color)
 *   @note              1, 叠加显示(仅显示需要显示的点, 不需要显示的点, 不做处理)
 * @param       color : 字体颜色
 * @retval      无
 */
void text_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *str, uint8_t size, uint8_t mode, uint16_t color)
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint8_t bHz = 0;                /* 字符或者中文 */
    uint8_t *pstr = (uint8_t *)str; /* 指向char*型字符串首地址 */
#if 0
    uint16_t str_len = strlen(str);
    LOG_I("str len = %d data:", str_len);
    for(uint16_t i = 0; i < str_len; i++)
    {
        LOG_I_NOTICK("%02x ", str[i]);
    }
    LOG_I_NOTICK("\n");
#endif
    while (*pstr != 0)          /* 数据未结束 */
    {
        if (!bHz)
        {
            if (*pstr > 0x80)   /* 中文 */
            {
                bHz = 1;        /* 标记是中文 */
            }
            else                /* 字符 */
            {
                if (x > (x0 + width - size / 2))        /* 换行 */
                {
                    y += size;
                    x = x0;
                }

                if (y > (y0 + height - size)) break;    /* 越界返回 */

                if (*pstr == 13)    /* 换行符号 */
                {
                    y += size;
                    x = x0;
                    pstr++;
                }
                else
                {
                    lcd_show_char(x, y, *pstr, size, mode, color);  /* 有效部分写入 */
                }

                pstr++;

                x += size / 2;      /* 英文字符宽度, 为中文汉字宽度的一半 */
            }
        }
        else            /* 中文 */
        {
            bHz = 0;    /* 有汉字库 */

            if (x > (x0 + width - size))   /* 换行 */
            {
                y += size;
                x = x0;
            }

            if (y > (y0 + height - size)) break;            /* 越界返回 */

            text_show_font(x, y, pstr, size, mode, color);  /* 显示这个汉字,空心显示 */
            pstr += 2;
            x += size;      /* 下一个汉字偏移 */
        }
    }
}

/**
 * @brief       在指定宽度的中间显示字符串
 *   @note      如果字符长度超过了len,则用text_show_string_middle显示
 * @param       x,y   : 起始坐标
 * @param       str   : 字符串
 * @param       size  : 字体大小
 * @param       width : 显示区域宽度
 * @param       color : 字体颜色
 * @retval      无
 */
void text_show_string_middle(uint16_t x, uint16_t y, char *str, uint8_t size, uint16_t width, uint16_t color)
{
    // uint16_t strlenth = 0;
    // strlenth = strlen((const char *)str);
    // strlenth *= size / 2;

    // if (strlenth > width) /* 超过了, 不能居中显示 */
    // {
    //     text_show_string(x, y, lcddev.width, lcddev.height, str, size, 1, color);
    // }
    // else
    // {
    //     strlenth = (width - strlenth) / 2;
    //     text_show_string(strlenth + x, y, lcddev.width, lcddev.height, str, size, 1, color);
    // }
}

/**
 * @brief       在指定位置显示一个字符
 * @param       x,y  : 坐标
 * @param       chr  : 要显示的字符:" "--->"~"
 * @param       size : 字体大小 12/16/24/32
 * @param       mode : 叠加方式(1); 非叠加方式(0);
 * @param       color : 字符的颜色;
 * @retval      无
 */
void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';    /* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];  /* 调用1206字体 */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];  /* 调用1608字体 */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];  /* 调用2412字体 */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];  /* 调用3216字体 */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];    /* 获取字符的点阵数据 */

        for (t1 = 0; t1 < 8; t1++)   /* 一个字节8个点 */
        {
            if (temp & 0x80)        /* 有效点,需要显示 */
            {
                lcd_draw_point(x, y, color);        /* 画点出来,要显示这个点 */
            }
            else if (mode == 0)     /* 无效点,不显示 */
            {
                lcd_draw_point(x, y, g_back_color); /* 画背景色,相当于这个点不显示(注意背景色由全局变量控制) */
            }

            temp <<= 1; /* 移位, 以便获取下一个位的状态 */
            y++;

            if (y >= lcd_dev.height)return;  /* 超区域了 */

            if ((y - y0) == size)   /* 显示完一列了? */
            {
                y = y0; /* y坐标复位 */
                x++;    /* x坐标递增 */

                if (x >= lcd_dev.width)return;   /* x坐标超区域了 */

                break;
            }
        }
    }
}











