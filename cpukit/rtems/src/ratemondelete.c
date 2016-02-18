/**
 *  @file
 *
 *  @brief RTEMS Delete Rate Monotonic
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_rate_monotonic_delete(
  rtems_id id
)
{
  Rate_monotonic_Control *the_period;
  Objects_Locations       location;
  ISR_Level               level;

  _Objects_Allocator_lock();
  the_period = _Rate_monotonic_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Scheduler_Release_job( the_period->owner, 0 );
      _Objects_Close( &_Rate_monotonic_Information, &the_period->Object );
      _ISR_Disable( level );
      _Watchdog_Per_CPU_remove_relative( &the_period->Timer );
      _ISR_Enable( level );
      the_period->state = RATE_MONOTONIC_INACTIVE;
      _Objects_Put( &the_period->Object );
      _Rate_monotonic_Free( the_period );
      _Objects_Allocator_unlock();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  _Objects_Allocator_unlock();

  return RTEMS_INVALID_ID;
}
