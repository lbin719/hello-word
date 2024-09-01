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

#define STRING_DISH_LEN                 (16)
#define STRING_NUM_LEN                  (6)
#define STRING_UNIT_LEN                 (7)
#define STRING_DEVICENUM_LEN            (4)
#define STRING_STATUS_LEN               (8)
#define STRING_USERNUM_LEN              (10)

typedef struct
{
    char dish_str[STRING_DISH_LEN + 1];

    char price_str[STRING_NUM_LEN + 1];// 单价
    char price_unit_str[STRING_UNIT_LEN + 1];

    char weight_str[STRING_NUM_LEN + 1];// 重量
    char weight_unit_str[STRING_UNIT_LEN + 1];

    char sum_price_str[STRING_NUM_LEN + 1];//设备总价
    // char sum_price_unit_str[STRING_UNIT_LEN + 1];

    char sumsum_price_str[STRING_NUM_LEN + 1];//消费总额
    // char sumsum_price_unit_str[STRING_UNIT_LEN + 1];

    char devicenum_str[STRING_DEVICENUM_LEN + 1];

    char status_str[STRING_STATUS_LEN + 1];

    char usernum_str[STRING_USERNUM_LEN + 1];
}ui_draw_t;

extern ui_draw_t ui_draw;

void set_draw_update_bit(uint32_t data_bit);

#endif /* _UI_TASK_H */
