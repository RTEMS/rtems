/**
 * @file
 *
 * @brief Returns the system uptime in seconds.
 * @ingroup ClassicClock Clocks
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
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

time_t rtems_clock_get_uptime_seconds( void )
{
  TOD_Control       *tod = &_TOD;
  Timestamp_Control  snapshot_as_timestamp;
  struct timespec    snapshot_as_timespec;
  ISR_Level          level;

  _TOD_Acquire( tod, level );
  snapshot_as_timestamp = tod->uptime;
  _TOD_Release( tod, level );

  _Timestamp_To_timespec( &snapshot_as_timestamp, &snapshot_as_timespec );

  return snapshot_as_timespec.tv_sec;
}
