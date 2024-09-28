#ifndef _WTN6040_H
#define _WTN6040_H

#include "stdint.h"

#define WTN_KSBH_PLAY           0x01
#define WTN_BHWC_PLAY           0x02
#define WTN_CPQHCG_PLAY         0x03
#define WTN_CPHWBD_PLAY         0x04
#define WTN_SMCGQQC_PLAY        0x05
#define WTN_WSBDCP_PLAY         0x06


void wtn6040_play(uint8_t index);
void wtn6040_set_voice(uint8_t level);
void wtn6040_init(void);

#endif /* _WTN6040_H */

