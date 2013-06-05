/**
 *  @file
 *
 *  @brief RTEMS Delete Timer
 *  @ingroup ClassicTimer
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

rtems_status_code rtems_timer_delete(
  rtems_id id
)
{
  Timer_Control     *the_timer;
  Objects_Locations  location;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      _Objects_Close( &_Timer_Information, &the_timer->Object );
      (void) _Watchdog_Remove( &the_timer->Ticker );
      _Timer_Free( the_timer );
      _Objects_Put( &the_timer->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
