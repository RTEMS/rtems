/*
 *  Time of Day (TOD) Handler -- Tickle Ticks
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

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/config.h>

/*
 *  _TOD_Tickle_ticks
 *
 *  This routine processes a clock tick.
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 */

void _TOD_Tickle_ticks( void )
{
  Timestamp_Control tick;
  uint32_t          nanoseconds_per_tick;

  nanoseconds_per_tick = rtems_configuration_get_nanoseconds_per_tick();

  /* Convert the tick quantum to a timestamp */
  _Timestamp_Set( &tick, 0, nanoseconds_per_tick );

  /* Update the counter of ticks since boot */
  _Watchdog_Ticks_since_boot += 1;

  /* Update the uptime */
  _Timestamp_Add_to( &_TOD.uptime, &tick );
  /* we do not care how much the uptime changed */

  /* Update the current TOD */
  _Timestamp_Add_to( &_TOD.now, &tick );

  _TOD.seconds_trigger += nanoseconds_per_tick;
  if ( _TOD.seconds_trigger >= 1000000000UL ) {
    _TOD.seconds_trigger -= 1000000000UL;
    _Watchdog_Tickle_seconds();
  }
}

