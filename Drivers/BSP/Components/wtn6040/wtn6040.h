#ifndef _WTN6040_H
#define _WTN6040_H

#include "stdint.h"

void wtn6040_task_handle(void);
void wtn6040_write_data(uint8_t data);
void wtn6040_init(void);

#endif /* _WTN6040_H */

