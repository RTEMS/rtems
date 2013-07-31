/**
 * @file
 *
 * @brief Increments time of day at each clock tick
 *
 * @ingroup ScoreTODConstants
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

void _TOD_Tickle_ticks( void )
{
  TOD_Control       *tod = &_TOD;
  ISR_Level          level;
  Timestamp_Control  tick;
  uint32_t           nanoseconds_per_tick;

  nanoseconds_per_tick = rtems_configuration_get_nanoseconds_per_tick();

  /* Convert the tick quantum to a timestamp */
  _Timestamp_Set( &tick, 0, nanoseconds_per_tick );

  /* Update the counter of ticks since boot */
  _Watchdog_Ticks_since_boot += 1;

  _TOD_Acquire( tod, level );

  /* Update the uptime */
  _Timestamp_Add_to( &tod->uptime, &tick );

  /* Update the current TOD */
  _Timestamp_Add_to( &tod->now, &tick );

  _TOD_Release( tod, level );

  _TOD.seconds_trigger += nanoseconds_per_tick;
  if ( _TOD.seconds_trigger >= 1000000000UL ) {
    _TOD.seconds_trigger -= 1000000000UL;
    _Watchdog_Tickle_seconds();
  }
}

