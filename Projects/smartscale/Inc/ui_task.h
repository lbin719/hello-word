#ifndef _UI_TASK_H
#define _UI_TASK_H

#include "stdint.h"


#define DISPLAY_DEBUG_INFO              1
#define DEBUG_INFO_TIME                 (200)


#define UI_TASK_NOTIFY                  (0xFFFFFFFFUL)

#define UI_NOTIFY_LOCK_BIT              (0x1UL << 0)
#define UI_NOTIFY_DISH_BIT              (0x1UL << 1)       
#define UI_NOTIFY_PRICE_BIT             (0x1UL << 2)  
#define UI_NOTIFY_PRICE_UNIT_BIT        (0x1UL << 3)
#define UI_NOTIFY_WEIGHT_BIT            (0x1UL << 4)
#define UI_NOTIFY_WEIGHT_UNIT_BIT       (0x1UL << 5)
#define UI_NOTIFY_SUM_PRICE_BIT         (0x1UL << 6)
#define UI_NOTIFY_SUMSUM_PRICE_BIT      (0x1UL << 7)
#define UI_NOTIFY_DEVICENUM_BIT         (0x1UL << 8)
#define UI_NOTIFY_STATUS_BIT            (0x1UL << 9)
#define UI_NOTIFY_USERNUM_BIT           (0x1UL << 10)
#define UI_NOTIFY_SIGNEL_BIT            (0x1UL << 11)
#define UI_NOTIFY_DEBUGINFO_BIT         (0x1UL << 12)

#define UI_NOTIFY_ALL_BIT               (0xFFFFUL)

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

extern const caiping_data_t default_caiping_data;
extern caiping_data_t caiping_data;

int32_t ui_ossignal_notify(int32_t signals);
extern void ui_init(void);

#endif /* _UI_TASK_H */
