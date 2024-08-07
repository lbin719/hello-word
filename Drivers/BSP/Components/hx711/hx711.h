#ifndef __HX711_H
#define __HX711_H

// #include "sys.h"


extern void hx711_init(void);
extern uint32_t hx711_read(void);
extern void hx711_get_maopi(void);
extern void hx711_get_weight(void);

extern uint32_t HX711_Buffer;
extern uint32_t Weight_Maopi;
extern int32_t Weight_Shiwu;

#endif

