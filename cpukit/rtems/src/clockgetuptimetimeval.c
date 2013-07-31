/**
 *  @file
 *
 *  @brief Gets the System Uptime in the Struct Timeval Format
 *  @ingroup ClassicClock
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

void rtems_clock_get_uptime_timeval( struct timeval *uptime )
{
  Timestamp_Control snapshot_as_timestamp;

  _TOD_Get_uptime( &snapshot_as_timestamp );
  _Timestamp_To_timeval( &snapshot_as_timestamp, uptime );
}
