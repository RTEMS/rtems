/**
 * @file
 *
 * @ingroup ClassicClock Clocks
 *
 * @brief Returns the system uptime in seconds.
 */

/*
 * Copyright (c) 2013 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>
#include <rtems/score/timestampimpl.h>
#include <rtems/score/todimpl.h>

uint64_t rtems_clock_get_uptime_nanoseconds( void )
{
  Timestamp_Control  snapshot_as_timestamp;

  _TOD_Get_zero_based_uptime(&snapshot_as_timestamp);

  return _Timestamp_Get_as_nanoseconds(&snapshot_as_timestamp);
}
