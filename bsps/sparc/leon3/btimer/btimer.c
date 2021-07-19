/*  timer.c
 *
 *  This file implements a benchmark timer using timer 2.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Ported to LEON implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  LEON modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */


#include <leon.h>
#include <rtems/btimer.h>

#if defined(RTEMS_MULTIPROCESSING)
  #define LEON3_TIMER_INDEX \
      ((rtems_configuration_get_user_multiprocessing_table()) ? \
        (rtems_configuration_get_user_multiprocessing_table()->node) - 1 : 1)
#else
  #define LEON3_TIMER_INDEX 0
#endif

bool benchmark_timer_find_average_overhead;

bool benchmark_timer_is_initialized = false;

void benchmark_timer_initialize(void)
{
  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */
  if (LEON3_Timer_Regs) {
    gptimer_timer *timer = &LEON3_Timer_Regs->timer[LEON3_TIMER_INDEX];
    if ( benchmark_timer_is_initialized == false ) {
      /* approximately 1 us per countdown */
      grlib_store_32( &timer->trldval, 0xffffff );
      grlib_store_32( &timer->tcntval, 0xffffff );
    } else {
      benchmark_timer_is_initialized = true;
    }
    grlib_store_32( &timer->tctrl, GPTIMER_TCTRL_EN | GPTIMER_TCTRL_LD );
  }
}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /*     to start/stop the timer. */
#define LEAST_VALID       2  /* Don't trust a value lower than this */

benchmark_timer_t benchmark_timer_read(void)
{
  uint32_t total;

  if (LEON3_Timer_Regs) {
    total =
      grlib_load_32( &LEON3_Timer_Regs->timer[LEON3_TIMER_INDEX].tcntval );

    total = 0xffffff - total;

    if ( benchmark_timer_find_average_overhead == true )
      return total;          /* in one microsecond units */

    if ( total < LEAST_VALID )
      return 0;            /* below timer resolution */

    return total - AVG_OVERHEAD;
  }
  return 0;
}

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
)
{
  benchmark_timer_find_average_overhead = find_flag;
}
