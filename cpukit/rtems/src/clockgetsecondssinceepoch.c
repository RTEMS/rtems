/**
 *  @file
 *
 *  @brief Obtain Seconds Since Epoch
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>

rtems_status_code rtems_clock_get_seconds_since_epoch(
  rtems_interval *the_interval
)
{
  if ( !the_interval )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  *the_interval = _TOD_Seconds_since_epoch();
  return RTEMS_SUCCESSFUL;
}
