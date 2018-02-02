/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>
#include <rtems.h>
#include <rtems/score/threadimpl.h>

static uint_fast32_t estimate_busy_loop_maximum( void )
{
  uint_fast32_t units = 0;
  uint_fast32_t initial = rtems_clock_get_ticks_since_boot();

  while ( initial == rtems_clock_get_ticks_since_boot() ) {
    ++units;
  }

  return units;
}

static uint_fast32_t wait_for_tick_change( void )
{
  uint_fast32_t initial = rtems_clock_get_ticks_since_boot();
  uint_fast32_t now;

  do {
    now = rtems_clock_get_ticks_since_boot();
  } while ( now == initial );

  return now;
}

/*
 * It is important that we use actually use the same rtems_test_busy() function
 * at the various places, since otherwise the obtained maximum value might be
 * wrong.  So, the compiler must not inline this function.
 */
RTEMS_NO_INLINE void rtems_test_busy( uint_fast32_t count )
{
  uint_fast32_t i = 0;

  do {
    __asm__ volatile ("");
    ++i;
  } while ( i < count );
}

uint_fast32_t rtems_test_get_one_tick_busy_count( void )
{
  uint_fast32_t last;
  uint_fast32_t now;
  uint_fast32_t a;
  uint_fast32_t b;
  uint_fast32_t m;

  /* Choose a lower bound */
  a = 1;

  /* Estimate an upper bound */

  wait_for_tick_change();
  b = 2 * estimate_busy_loop_maximum();

  while ( true ) {
    last = wait_for_tick_change();
    rtems_test_busy( b );
    now = rtems_clock_get_ticks_since_boot();

    if ( now != last ) {
      break;
    }

    b *= 2;
    last = now;
  }

  /* Find a good value */
  do {
    m = ( a + b ) / 2;

    last = wait_for_tick_change();
    rtems_test_busy( m );
    now = rtems_clock_get_ticks_since_boot();

    if ( now != last ) {
      b = m;
    } else {
      a = m;
    }
  } while ( b - a > 1 );

  return m;
}

void rtems_test_busy_cpu_usage( time_t seconds, long nanoseconds )
{
  Thread_Control    *executing;
  Timestamp_Control  busy;
  Timestamp_Control  start;
  Timestamp_Control  now;

  executing = _Thread_Get_executing();
  _Thread_Get_CPU_time_used( executing, &start );
  _Timestamp_Set( &busy, seconds, nanoseconds );

  do {
    _Thread_Get_CPU_time_used( executing, &now );
  } while ( now - start < busy );
}
