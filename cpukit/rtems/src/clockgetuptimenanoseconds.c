/**
 * @file
 *
 * @brief Returns the system uptime in seconds.
 * @ingroup ClassicClock Clocks
 */

/*
 * Copyright (c) 2013 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>

uint64_t rtems_clock_get_uptime_nanoseconds( void )
{
  Timestamp_Control  snapshot_as_timestamp;
  uint32_t           nanoseconds;
  ISR_Level          level;

  _TOD_Acquire( &_TOD, level );
    snapshot_as_timestamp = _TOD.uptime;
    nanoseconds = ( *_TOD.nanoseconds_since_last_tick )();
  _TOD_Release( &_TOD, level );

  return _Timestamp_Get_As_nanoseconds( &snapshot_as_timestamp, nanoseconds );
}
