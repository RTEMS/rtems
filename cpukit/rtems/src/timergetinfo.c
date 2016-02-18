/**
 *  @file
 *
 *  @brief RTEMS Get Timer Information
 *  @ingroup ClassicTimer
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/watchdog.h>

rtems_status_code rtems_timer_get_information(
  rtems_id                 id,
  rtems_timer_information *the_info
)
{
  Timer_Control     *the_timer;
  Objects_Locations  location;
  ISR_lock_Context   lock_context;
  Per_CPU_Control   *cpu;

  if ( !the_info )
    return RTEMS_INVALID_ADDRESS;

  the_timer = _Timer_Get( id, &location, &lock_context );
  switch ( location ) {

    case OBJECTS_LOCAL:
      cpu = _Timer_Acquire_critical( the_timer, &lock_context );
      the_info->the_class  = the_timer->the_class;
      the_info->initial    = the_timer->initial;
      the_info->start_time = the_timer->start_time;
      the_info->stop_time  = the_timer->stop_time;
      _Timer_Release( cpu, &lock_context );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
