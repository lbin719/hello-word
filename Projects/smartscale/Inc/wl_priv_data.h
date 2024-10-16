#ifndef __WL_PRIV_DATA_H
#define __WL_PRIV_DATA_H

#include "stdbool.h"
#include "stdint.h"

typedef bool (*priv_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
    int cmd;
    priv_func_t func;
} priv_func_mapping_t;

extern bool wlpriv_banpan_result;
extern uint32_t hx711_cali_value;
bool wl_rx_priv_parse(int argc, char *argv[]);

#endif