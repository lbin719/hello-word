#ifndef _RTC_TIMER_H
#define _RTC_TIMER_H

#include <stdint.h>
#include <stdbool.h>



typedef struct rtc_time_t_
{
    uint8_t sec;     /**< DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59 */
    uint8_t min;     /**< DEC format:value range from 0 to 59, BCD format:value range from 0x00 to 0x59 */
    uint8_t hour;    /**< DEC format:value range from 0 to 23, BCD format:value range from 0x00 to 0x23 */
    uint8_t wday;    /**< DEC or BCD format:value range from 0 to  6, from 0(Sunday) to 6(Saturday)*/
    uint8_t mday;     /**< DEC format:value range from 1 to 31, BCD format:value range from 0x01 to 0x31 */
    uint8_t mon;   /**< DEC format:value range from 1 to 12, BCD format:value range from 0x01 to 0x12 */
    uint16_t year;   /**< DEC format:value range from 0 to 9999, BCD format:value range from 0x0000 to 0x9999 */
} rtc_time_t;

typedef struct
{
    uint32_t    sec;
    uint32_t    usec;
} clock_time_t;

typedef struct
{
    uint16_t    year;
    uint8_t     month;
    uint8_t     day;
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
    uint16_t    subsec;
    uint8_t     wday;
    int8_t      timezone;
} clock_date_t;

extern volatile uint32_t ms_count;
extern volatile uint32_t timestamp;

uint32_t date_to_seconds(const clock_date_t *date);
void seconds_to_date(uint32_t seconds, clock_date_t *date);

uint32_t get_timestamp(void);
void set_timestamp(uint32_t data);

#endif /* _RTC_TIMER_H */
