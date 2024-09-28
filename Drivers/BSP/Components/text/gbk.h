#ifndef __GBK_H
#define __GBK_H

#define UI_DEFAULT_CAIPING      "小鸡炖的蘑菇"

#define UI_DANJIA_STR           "单    价:"
#define UI_ZHONGLIANG_STR       "重    量:"
#define UI_ZONGJIA_STR          "总    价:"
#define UI_XFZE_STR             "消费总额:"

#define UI_YUAN_STR             "元"
#define UI_KE_STR               "克(g)"
#define UI_ZZDL_STR             "正在登录"
#define UI_HYSY_STR             "欢迎使用"


char *const ld_str[] = { 
  "正在登录",
  "设备离线", 
  "设备正常",
  "补货中",
  "请绑定餐盘",
  "请取餐",
};

#if 0
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

ui_draw_t ui_draw = {
  .dish_str = "萝卜炒肉",

  .price_str = "0.30",
  .price_unit_str = "元\/100g",

  .weight_str = "0",
  .weight_unit_str = "克(g)",

  .sum_price_str = "0.00",
  .sumsum_price_str = "0.00",

  .devicenum_str = "01",
  .status_str = "正在登录",
  .usernum_str = "欢迎使用",
};

#endif

#endif