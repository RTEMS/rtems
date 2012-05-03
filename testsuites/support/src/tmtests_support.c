/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/btimer.h>

#include <bsp.h>
#include <rtems/timerdrv.h>
#include "test_support.h"
#include "timesys.h"
#include "tmtests_empty_function.h"

void rtems_time_test_measure_operation(
  const char               *description,
  rtems_time_test_method_t  operation,
  void                     *argument,
  int                       iterations,
  int                       overhead
)
{
  int  i;
  uint32_t loop_overhead;
  uint32_t end_time;

  benchmark_timer_initialize();
    for (i=0 ; i<iterations ; i++ ) {
      benchmark_timer_empty_operation( i, argument );
    }
  loop_overhead = benchmark_timer_read();

  benchmark_timer_initialize();
    for (i=0 ; i<iterations ; i++ ) {
      (*operation)( i, argument );
    }
  end_time = benchmark_timer_read();

  put_time(
    description,
    end_time,
    iterations,
    loop_overhead,
    overhead
  );
}
