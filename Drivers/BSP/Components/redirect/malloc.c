/*
 * =====================================================================================
 *
 *  Copyright (C) 2016. Huami Ltd, unpublished work. This computer program includes
 *  Confidential, Proprietary Information and is a Trade Secret of Huami Ltd.
 *  All use, disclosure, and/or reproduction is prohibited unless authorized in writing.
 *  All Rights Reserved.
 *
 *  Author:  yuanxiaochen@huami.com
 *
 * =====================================================================================
 */


#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"

void *malloc(size_t size)
{
//    if (size == 0)
//    {
//        return 0;
//    }
//    return pvPortMalloc(size);
}

void *calloc(size_t nelem, size_t elsize)
{
//    if (nelem * elsize == 0)
//    {
//        return 0;
//    }
//
//    void *ptr = pvPortMalloc(nelem * elsize);
//
//    if (ptr)
//    {
//        memset(ptr, 0, nelem * elsize);
//    }
//
//    return ptr;
}

void *realloc(void *ptr, size_t size)
{
//    return pvPortRealloc(ptr, size);
}

void free(void *ptr)
{
//    if (!ptr)
//        return;
//
//    return vPortFree(ptr);
}
