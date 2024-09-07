#ifndef _WTN6040_H
#define _WTN6040_H

#include "stdint.h"

void wtn6040_play(uint8_t index);
void wtn6040_set_voice(uint8_t level);
void wtn6040_task_handle(void);
void wtn6040_init(void);

#endif /* _WTN6040_H */

