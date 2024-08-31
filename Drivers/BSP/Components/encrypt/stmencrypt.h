#ifndef __STMENCRYPT_H
#define __STMENCRYPT_H

#include "stdint.h"
#include "stdbool.h"


// write stmflash need 4 bytes aignment
typedef struct {
	uint32_t header;
	uint32_t tick;
    uint32_t key;
} stmencrypt_data_t;


bool get_stmencrypt_status(void);
uint32_t stmencrypt_write_key(void);
void stmencrypt_init(void);

#endif


















