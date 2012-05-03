/*  timer.c
 *
 *  This file implements a benchmark timer using timer 2.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to LEON implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  LEON modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */


#include <bsp.h>
#include <rtems/btimer.h>

bool benchmark_timer_find_average_overhead;

bool benchmark_timer_is_initialized = false;

void benchmark_timer_initialize(void)
{
  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */

  if ( benchmark_timer_is_initialized == false ) {

    /* approximately 1 us per countdown */
    LEON_REG.Timer_Counter_2 = 0xffffff;
    LEON_REG.Timer_Reload_2 = 0xffffff;

  } else {
    benchmark_timer_is_initialized = true;
  }

  LEON_REG.Timer_Control_2 = (
    LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |
      LEON_REG_TIMER_COUNTER_LOAD_COUNTER
  );

}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /*     to start/stop the timer. */
#define LEAST_VALID       2  /* Don't trust a value lower than this */

uint32_t benchmark_timer_read(void)
{
  uint32_t total;

  total = LEON_REG.Timer_Counter_2;

  total = 0xffffff - total;

  if ( benchmark_timer_find_average_overhead == true )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
