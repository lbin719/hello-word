#ifndef __GBK_H
#define __GBK_H

#define UI_DEFAULT_CAIPING      "С������Ģ��"

#define UI_DANJIA_STR           "��    ��:"
#define UI_ZHONGLIANG_STR       "��    ��:"
#define UI_ZONGJIA_STR          "��    ��:"
#define UI_XFZE_STR             "�����ܶ�:"

#define UI_YUAN_STR             "Ԫ"
#define UI_KE_STR               "��(g)"
#define UI_ZZDL_STR             "���ڵ�¼"
#define UI_HYSY_STR             "��ӭʹ��"


char *const ld_str[] = { 
  "���ڵ�¼",
  "�豸����", 
  "�豸����",
  "������",
  "��󶨲���",
  "��ȡ��",
};

#if 0
typedef struct
{
    char dish_str[STRING_DISH_LEN + 1];

    char price_str[STRING_NUM_LEN + 1];// ����
    char price_unit_str[STRING_UNIT_LEN + 1];

    char weight_str[STRING_NUM_LEN + 1];// ����
    char weight_unit_str[STRING_UNIT_LEN + 1];

    char sum_price_str[STRING_NUM_LEN + 1];//�豸�ܼ�
    // char sum_price_unit_str[STRING_UNIT_LEN + 1];

    char sumsum_price_str[STRING_NUM_LEN + 1];//�����ܶ�
    // char sumsum_price_unit_str[STRING_UNIT_LEN + 1];

    char devicenum_str[STRING_DEVICENUM_LEN + 1];

    char status_str[STRING_STATUS_LEN + 1];

    char usernum_str[STRING_USERNUM_LEN + 1];
}ui_draw_t;

ui_draw_t ui_draw = {
  .dish_str = "�ܲ�����",

  .price_str = "0.30",
  .price_unit_str = "Ԫ\/100g",

  .weight_str = "0",
  .weight_unit_str = "��(g)",

  .sum_price_str = "0.00",
  .sumsum_price_str = "0.00",

  .devicenum_str = "01",
  .status_str = "���ڵ�¼",
  .usernum_str = "��ӭʹ��",
};

#endif

#endif