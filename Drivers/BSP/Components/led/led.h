#ifndef _LED_H
#define _LED_H

#include "stdint.h"
#include "stdbool.h"

void led_control(bool on, uint32_t timer);
void led_init(void);


#endif /* _LED_H */

