/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This source file contains the implementation of
 *   rtems_clock_set().
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

#include <rtems/rtems/clockimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_clock_set(
  const rtems_time_of_day *tod
)
{
  rtems_status_code status;
  Status_Control    score_status;
  struct timespec   tod_as_timespec;
  ISR_lock_Context  lock_context;

  status = _TOD_Validate( tod, TOD_ENABLE_TICKS_VALIDATION );

  if ( status != RTEMS_SUCCESSFUL ) {
    return status;
  }

  tod_as_timespec.tv_sec = _TOD_To_seconds( tod );
  tod_as_timespec.tv_nsec = tod->ticks
    * rtems_configuration_get_nanoseconds_per_tick();

  _TOD_Lock();
  _TOD_Acquire( &lock_context );
  score_status = _TOD_Set( &tod_as_timespec, &lock_context );
  _TOD_Unlock();

  return _Status_Get( score_status );
}
