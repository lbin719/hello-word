#ifndef __FS_H
#define __FS_H

#include "stdbool.h"

//Windows drive letter can only be up to 11 characters long and cannot be defined as too long
#define MSC_LABLE_HEAD	"MSC-"

void fs_init(void);

#endif
