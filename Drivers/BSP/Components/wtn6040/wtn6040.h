#ifndef _WTN6040_H
#define _WTN6040_H

#include "stdint.h"

#define WTN6040_DEFAULT_VOICE   (15)

#define WTN6040_MAX_VOICE       (15)
#define WTN6040_MAX_PLAY        (8)





#define WTN_KSBH_PLAY           0x01 // 开始补货
#define WTN_BHWC_PLAY           0x02 // 补货完成
#define WTN_CPQHCG_PLAY         0x03 // 菜品切换成功
#define WTN_CPHWBD_PLAY         0x04 // 餐盘号未绑定
#define WTN_SMCGQQC_PLAY        0x05 // 扫码成功，请取菜
#define WTN_WSBDCP_PLAY         0x06 // 未识别到餐盘，请先放好餐盘再取菜

#define WTN_QPCZTX_PLAY         0x07 // 去皮操作时勿将菜品也放入装菜盘
#define WTN_QQKTPTX_PLAY        0x08 // 请清空托盘上的物品，正在为您清零
#define WTN_CZTQLTX_PLAY        0x09 // 称重台数据已归零

void wtn6040_play(uint8_t index);
void wtn6040_set_voice(uint8_t level);
void wtn6040_set_voice_store(uint8_t level);
void wtn6040_init(void);

#endif /* _WTN6040_H */

