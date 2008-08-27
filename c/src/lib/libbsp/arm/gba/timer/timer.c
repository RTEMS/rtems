/**
 *  @file timer.c
 *
 *  GBA Timer driver.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 * Notes:
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  Timer_initialize() and Read_timer().  Read_timer() usually returns
 *  the number of microseconds since Timer_initialize() exitted.
 *
 *  It is important that the timer start/stop overhead be determined
 *  when porting or modifying this code.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <irq.h>
#include <gba.h>

/*
 * Set up the timer hardware
 *    1 / 16.78Mhz  => 59.595 ns
 *   64 / 16.78Mhz  =>  3.814 us
 *  256 / 16.78Mhz  => 15.256 us
 * 1024 / 16.78Mhz  => 61.025 us
 */
#define  __TimePreScaler      1
#define  __TickTime_ns        ((1000000000L/__ClockFrequency)*__TimePreScaler)
#define  __TickTime_us        ((1000000L/__ClockFrequency)*__TimePreScaler)

#if (__TimePreScaler==1)
 #define GBA_TM0CNT_PS    0x0000
#elif (__TimePreScaler==64)
 #define GBA_TM0CNT_PS    0x0001
#elif (__TimePreScaler==256)
 #define GBA_TM0CNT_PS    0x0002
#elif (__TimePreScaler==1024)
 #define GBA_TM0CNT_PS    0x0003
#else
 #define GBA_TM0CNT_PS    0x0003
#endif

bool Timer_driver_Find_average_overhead;

/**
 *  @brief Timer_initialize start TM0 and TM1
 *
 *  @param  None
 *  @return None
 */
void Timer_initialize( void )
{
  GBA_REG_TM1CNT =  0x0000;                /* Stop Counters         */
  GBA_REG_TM0CNT =  0x0000;
  GBA_REG_TM1D   =  0x0000;                /* Reset Counters        */
  GBA_REG_TM0D   =  0x0000;
  /* Start Counters */
  GBA_REG_TM1CNT =  0x0084;                /* Start Count Up timing */
  GBA_REG_TM0CNT = (0x0080|GBA_TM0CNT_PS); /* Start Count           */
}

/*
 *  The following controls the behavior of Read_timer().
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD    3  /**< It typically takes 3 microseconds          */
#define LEAST_VALID     1  /**< Don't trust a clicks value lower than this */

/**
 *  @brief Read_timer return timer countervalue in microseconds.
 *
 *  Used in Timing Test Suite.
 *
 *  @param  None
 *  @return Timer value in microseconds
 */
uint32_t Read_timer( void )
{
  uint32_t  ticks;
  uint32_t  total;

  /* Stop Counters */
  GBA_REG_TM0CNT =  0x0000;
  GBA_REG_TM1CNT =  0x0000;
  /* Read Counters */
  ticks = (GBA_REG_TM1D<<16) | GBA_REG_TM0D;
  if ( ticks < LEAST_VALID ) {
      return 0;  /* below timer resolution */
  }
  /* convert to uS */
  total = ((ticks * __TickTime_ns) / 1000);
  if ( Timer_driver_Find_average_overhead == 1 ) {
      return total;          /* in microseconds */
  }
  else {
    if ( total < AVG_OVERHEAD ) {
        return 0;
    }
    return (total - AVG_OVERHEAD);
  }
}

/**
 *  @brief Empty function
 *
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 *
 *  @param  None
 *  @return RTEMS_SUCCESSFUL
*/
rtems_status_code Empty_function( void )
{
   return RTEMS_SUCCESSFUL;
}

/**
 *  @brief Set Timer_driver_Find_average_overhead flag.
 *
 *  Used in Timing Test Suite.
 *
 *  @param  find_flag boolean find_flag
 *  @return None
*/
void Set_find_average_overhead(bool find_flag)
{
   Timer_driver_Find_average_overhead = find_flag;
}
