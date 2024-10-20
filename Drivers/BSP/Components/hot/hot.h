#ifndef __HOT_H
#define __HOT_H

#include "stdint.h"

#define HOT_ON          (1)
#define HOT_OFF         (0)


void hot_ctrl_store(uint8_t mode, uint8_t time);

void hot_init(void);

#endif