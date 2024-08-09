
/*
 * =====================================================================================
 *
 *  Copyright (C) 2021. Huami Ltd, unpublished work. This computer program includes
 *  Confidential, Proprietary Information and is a Trade Secret of Huami Ltd.
 *  All use, disclosure, and/or reproduction is prohibited unless authorized in writing.
 *  All Rights Reserved.
 *
 *  Author:
 *
 * =====================================================================================
 */

#ifndef _CRYPTO_H_
#define _CRYPTO_H_


#include <stdint.h>


/**
 * @brief calc crc32
 *
 * @param crc initial crc value
 * @param data data to be calculate
 * @param len data length
 * @return uint32_t after crc32 update value
 */
uint32_t crypto_crc32_calc(uint32_t crc, const uint8_t *data, uint32_t len);

/**
 * Calculate the CRC32 value of a memory buffer.
 *
 * @param crc accumulated CRC32 value, must be 0 on first call
 * @param buf buffer to calculate CRC32 value for
 * @param size bytes in buffer
 *
 * @return calculated CRC32 value
 */
uint32_t crypto_ef_calc_crc32(uint32_t crc, const void *buf, uint32_t size);

#endif
