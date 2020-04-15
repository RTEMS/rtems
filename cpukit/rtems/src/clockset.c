/**
 *  @file
 *
 *  @brief Set the Current TOD
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/clock.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_clock_set(
  const rtems_time_of_day *tod
)
{
  Status_Control status;

  if ( !tod )
    return RTEMS_INVALID_ADDRESS;

  if ( _TOD_Validate( tod ) ) {
    struct timespec  tod_as_timespec;
    ISR_lock_Context lock_context;

    tod_as_timespec.tv_sec = _TOD_To_seconds( tod );
    tod_as_timespec.tv_nsec = tod->ticks
      * rtems_configuration_get_nanoseconds_per_tick();

    _TOD_Lock();
    _TOD_Acquire( &lock_context );
    status = _TOD_Set( &tod_as_timespec, &lock_context );
    _TOD_Unlock();

    return STATUS_GET_CLASSIC( status );
  }

  return RTEMS_INVALID_CLOCK;
}
