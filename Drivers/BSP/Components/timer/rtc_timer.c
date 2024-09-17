#include "rtc_timer.h"
#include "stdint.h"

#define SECONDS_PER_TIMEZONE    (15 * 60)

volatile uint32_t ms_count = 0;
volatile uint32_t timestamp = 0;

static int32_t timezone_to_seconds(int8_t tz)
{
    return tz * SECONDS_PER_TIMEZONE;
}

uint32_t date_to_seconds(const clock_date_t *date)
{
    uint16_t year = date->year;
    uint8_t month = date->month;
    uint8_t day = date->day;
    uint8_t hour = date->hour;
    uint8_t minute = date->minute;
    uint8_t second = date->second;
    uint32_t ts = 0;
    /* 二月闰月，计算当作最后一个月份 */
    month -= 2;

    if ((int8_t)month <= 0)
    {
        month += 12;
        year -= 1;
    }

    /* 计算时间戳 */
    ts = (((year / 4 - year / 100 + year / 400 + 367 * month / 12 + day + year * 365
            - 719499) * 24 +
           hour) * 60 + minute) * 60 + second;
    ts -= timezone_to_seconds(date->timezone);
    return ts;
}

void seconds_to_date(uint32_t seconds, clock_date_t *date)
{
    // uint32_t leapyears = 0, yearhours = 0;
    // const uint32_t mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // const uint16_t ONE_YEAR_HOURS = 8760;
    // memset(date, 0, sizeof(clock_date_t));
    // /* 周天 */
    // date->wday = ((seconds / (24 * 60 * 60)) + 3) % 7 + 1;
    // date->wday %= 7;    // .Sun as 0
    // /* 秒 */
    // date->second = seconds % 60;
    // seconds /= 60;
    // /* 分 */
    // date->minute = seconds % 60;
    // seconds /= 60;
    // /* 年 */
    // leapyears = seconds / (1461 * 24);
    // date->year = (leapyears << 2) + 1970;
    // seconds %= 1461 * 24;

    // for (;;)
    // {
    //     yearhours = ONE_YEAR_HOURS;

    //     /* 闰年加一天 */
    //     if ((date->year & 3) == 0)
    //     {
    //         yearhours += 24;
    //     }

    //     /* 剩余时间已经不足一年了跳出循环 */
    //     if (seconds < yearhours)
    //     {
    //         break;
    //     }

    //     date->year++;
    //     /* 减去一年的时间 */
    //     seconds -= yearhours;
    // }

    // /* 时 */
    // date->hour = seconds % 24;
    // seconds /= 24;
    // seconds++;

    // /* 闰年 */
    // if ((date->year & 3) == 0)
    // {
    //     if (seconds > 60)
    //     {
    //         seconds--;
    //     }
    //     else
    //     {
    //         if (seconds == 60)
    //         {
    //             date->month = 2;
    //             date->day = 29;
    //             return;
    //         }
    //     }
    // }

    // /* 月 */
    // for (date->month = 0; mdays[date->month] < seconds; date->month++)
    // {
    //     seconds -= mdays[date->month];
    // }

    // date->month++;
    // /* 日 */
    // date->day = seconds;
}

uint32_t get_timestamp(void)
{
    return timestamp;
}

void set_timestamp(uint32_t data)
{
    timestamp = data;
}