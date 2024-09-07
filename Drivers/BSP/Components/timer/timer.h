#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#define platformDelay(t)                    HAL_Delay(t)                                  /*!< Performs a delay for the given time (ms)    */
#define platformGetSysTick()                HAL_GetTick()                                 /*!< Get System Tick ( 1 tick = 1 ms)            */

#define timer_creat( t )                    timerCalculateTimer(t)                        /*!< Create a timer with the given time (ms)     */
#define timer_start(timer, time_ms)         do{(timer) = MIN((time_ms), 0x00FFFFFF) + platformGetSysTick(); } while(0)  /*!< Configures and starts timer         */
#define timer_isexpired(timer)              timerIsExpired( timer )                       /*!< Checks if timer has expired  */
#define timerIsRunning(t)                   (!timerIsExpired(t))

uint32_t timerCalculateTimer( uint16_t time );
bool timerIsExpired( uint32_t timer );
void timerDelay( uint16_t time );
void timerStopwatchStart( void );
uint32_t timerStopwatchMeasure( void );

#endif /* _TIMER_H */
