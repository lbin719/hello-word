#ifndef __HX711_H
#define __HX711_H

#define HX_DEFAULT_ZERO 		(8411213)
#define HX_DEFAULT_GAP          (109.578)

void hx711_set_zero(void);
void hx711_set_calibration(uint32_t weight);
uint32_t hx711_get_weight(void);
void hx711_init(void);

#endif

