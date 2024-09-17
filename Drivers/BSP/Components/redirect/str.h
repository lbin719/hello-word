
#ifndef __STR_H
#define __STR_H

#include "stdint.h"


int str_toint(const char* str);
int str_length(char *buf);
// char* str_find_number(char *str, uint16_t len, char *num_buf, uint16_t *num_len);
int str_char_offest(char *buf, char data);
int str_num_offest(char *str);

uint32_t str_split(char *str, uint32_t strsize, char *argv[], uint32_t argcm);

#endif





















