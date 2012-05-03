/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer on
 *  the MEC.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
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
    ERC32_MEC.General_Purpose_Timer_Scalar  = CLOCK_SPEED - 1;
    ERC32_MEC.General_Purpose_Timer_Counter = 0xffffffff;

  } else {
    benchmark_timer_is_initialized = true;
  }

  ERC32_MEC_Set_General_Purpose_Timer_Control(
    ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING |
      ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER
  );

  ERC32_MEC_Set_General_Purpose_Timer_Control(
    ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING
  );

}

#if ENABLE_SIS_QUIRKS
#define AVG_OVERHEAD      8  /* It typically takes 3.0 microseconds */
                             /*     to start/stop the timer. */
#define LEAST_VALID       9  /* Don't trust a value lower than this */
#else
#define AVG_OVERHEAD     12  /* It typically takes 3.0 microseconds */
                             /*     to start/stop the timer. */
#define LEAST_VALID      13  /* Don't trust a value lower than this */
#endif

uint32_t benchmark_timer_read(void)
{
  uint32_t          total;

  total = ERC32_MEC.General_Purpose_Timer_Counter;

  total = 0xffffffff - total;

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
