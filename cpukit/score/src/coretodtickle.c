/**
 * @file
 *
 * @brief Increments time of day at each clock tick
 *
 * @ingroup ScoreTOD
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

void _TOD_Tickle_ticks( void )
{
  /* Update the counter of ticks since boot */
  _Watchdog_Ticks_since_boot += 1;

  _TOD.seconds_trigger += rtems_configuration_get_nanoseconds_per_tick();
  if ( _TOD.seconds_trigger >= 1000000000UL ) {
    _TOD.seconds_trigger -= 1000000000UL;
    _Watchdog_Tickle_seconds();
  }
}

