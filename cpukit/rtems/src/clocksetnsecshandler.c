/**
 * @file
 *
 * @brief Set the BSP specific Nanoseconds Extension
 * @ingroup ClassicClock Clocks
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

rtems_status_code rtems_clock_set_nanoseconds_extension(
  rtems_nanoseconds_extension_routine routine
)
{
  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  _TOD_Set_nanoseconds_since_last_tick_handler( routine );

  return RTEMS_SUCCESSFUL;
}
