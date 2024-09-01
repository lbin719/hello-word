#ifndef __HX711_H
#define __HX711_H

void hx711_set_zero(void);
void hx711_set_calibration(uint32_t weight);
uint32_t hx711_get_weight(void);
void hx711_init(void);

#endif

