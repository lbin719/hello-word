#ifndef _UI_TASK_H
#define _UI_TASK_H

#include "stdint.h"

#define DRAW_UPDATE_DISH_BIT            (0x1UL << 0)       
#define DRAW_UPDATE_PRICE_BIT           (0x1UL << 1)  
#define DRAW_UPDATE_PRICE_UNIT_BIT      (0x1UL << 2)
#define DRAW_UPDATE_WEIGHT_BIT          (0x1UL << 3)
#define DRAW_UPDATE_WEIGHT_UNIT_BIT     (0x1UL << 4)
#define DRAW_UPDATE_SUM_PRICE_BIT       (0x1UL << 5)
#define DRAW_UPDATE_SUMSUM_PRICE_BIT    (0x1UL << 6)
#define DRAW_UPDATE_DEVICENUM_BIT       (0x1UL << 7)
#define DRAW_UPDATE_STATUS_BIT          (0x1UL << 8)
#define DRAW_UPDATE_USERNUM_BIT         (0x1UL << 9)
#define DRAW_UPDATE_SIGNEL_BIT          (0x1UL << 11)

#define DRAW_UPDATE_LOCK_BIT            (0x1UL << 30)
#define DRAW_UPDATE_ALL_BIT             (0x1UL << 31)

#define STRING_DISH_LEN                 (12)
#define STRING_NUM_LEN                  (6)
#define STRING_UNIT_LEN                 (7)
#define STRING_DEVICENUM_LEN            (4)
#define STRING_STATUS_LEN               (8)
#define STRING_USERNUM_LEN              (10)

typedef struct
{
    char dish_str[STRING_DISH_LEN + 1];// 菜品名称

    uint8_t mode; // 称重模式

    float price;// 菜品售价
    uint32_t price_unit;// 菜品售价重量

    uint32_t tool_weight; // 打菜工具重量
    uint32_t zhendongwucha; // 允许震动误差

    uint32_t devicenum; // 设备编号

}caiping_data_t;

extern caiping_data_t default_caiping_data;

void set_draw_update_bit(uint32_t data_bit);

#endif /* _UI_TASK_H */
