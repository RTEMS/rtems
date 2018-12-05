/* 
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "test_support.h"

/*
 *  Burn CPU for specified number of ticks
 */
void rtems_test_spin_for_ticks(rtems_interval ticks)
{
  rtems_interval start;
  rtems_interval now;

  start = rtems_clock_get_ticks_since_boot();
  do {
    now = rtems_clock_get_ticks_since_boot();
  } while ( now - start < ticks );
}

/*
 *  Spin until the next clock tick
 */
void rtems_test_spin_until_next_tick( void )
{
  rtems_interval start;
  rtems_interval now;

  start = rtems_clock_get_ticks_since_boot();
  do {
    now = rtems_clock_get_ticks_since_boot();
  } while ( now == start );
}
