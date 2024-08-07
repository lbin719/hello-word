/**
 ****************************************************************************************************
 * @file        font.c
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

#include "string.h"
#include "stdint.h"
#include "ff.h"
#include "ulog.h"
#include "fonts.h"

/* 字库区域占用的总扇区数大小(3�?���?+unigbk�?+字库信息=3238700 字节,约占791�?25QXX扇区,一�?���?4K字节) */
#define FONTSECSIZE         791


/* 字库存放起�?地址
 * 从�?4.2MB地址开始存放字�?
 * 前面4.2MB大小�?��件系统占�?
 * 4.2MB后紧�?3�?���?+UNIGBK.BIN,总大�?3.09M, 791�?���?,�?��库占用了,不能�?!
 * 7.29M以后, 用户�?���?��使用. 大�?�?700K字节的空�?
 */
#define FONTINFOADDR        4.2 * 1024 * 1024

 
/* 用来保存字库基本信息，地址，大小等 */
_font_info ftinfo;

/* 字库存放在�?盘中的路�? */
char *const FONT_GBK_PATH[] =
{
    // "UNIGBK.BIN",      /* UNIGBK.BIN的存放位�? */
    "GBK12.FON",       /* GBK12的存放位�? */
    "GBK16.FON",       /* GBK16的存放位�? */
    "GBK24.FON",       /* GBK24的存放位�? */
};

/* 更新时的提示信息 */
char *const FONT_UPDATE_REMIND_TBL[] =
{
    // "Updating UNIGBK.BIN",          /* 提示正在更新UNIGBK.bin */
    "Updating GBK12.FON ",          /* 提示正在更新GBK12 */
    "Updating GBK16.FON ",          /* 提示正在更新GBK16 */
    "Updating GBK24.FON ",          /* 提示正在更新GBK24 */
};


/**
 * @brief       初�?化字�?
 * @param       �?
 * @retval      0, 字库完好; 其他, 字库丢失;
 */
bool fonts_init(void)
{
    // uint8_t t = 0;

    // while (t < 10)  /* 连续读取10�?,都是错�?,说明�?���?���??,得更新字库了 */
    // {
    //     t++;
    //     norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo)); /* 读出ftinfo结构体数�? */

    //     if (ftinfo.fontok == 0XAA)
    //     {
    //         break;
    //     }
        
    //     delay_ms(20);
    // }

    // if (ftinfo.fontok != 0XAA)
    // {
    //     return 1;
    // }

	FIL File;
    for(uint8_t i = 0; i < 3; i++)
    {
        if (f_open(&File, FONT_GBK_PATH[i], FA_READ) != FR_OK)
        {
            Error_Handler();
            LOG_I("f_open:%s fail!!!\r\n", FONT_GBK_PATH[i]);
            return false;
        }
        else
        {
            LOG_I("f_open:%s success\r\n", FONT_GBK_PATH[i]);
            f_close(&File);
        }

    }

    return true;
}












