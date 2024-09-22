#ifndef __HOT_H
#define __HOT_H

#include "stdint.h"

#define HOT_ON          (1)
#define HOT_OFF         (0)


void hot_ctrl(uint8_t ctrl);

void hot_init(void);

#endif