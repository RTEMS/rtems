/*
 *  Timer Manager - rtems_timer_get_information directive
 *
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  rtems_timer_get_information
 *
 *  This directive allows a thread to obtain information about a timer.
 *
 *  Input parameters:
 *    id       - timer id
 *    the_info - pointer to timer information block
 *
 *  Output parameters:
 *    *the_info        - region information block filled in
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 * 
 */

rtems_status_code rtems_timer_get_information(
  Objects_Id               id,
  rtems_timer_information *the_info
)
{
  Timer_Control     *the_timer;
  Objects_Locations  location;

  if ( !the_info )
    return RTEMS_INVALID_ADDRESS;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      the_info->the_class  = the_timer->the_class;
      the_info->initial    = the_timer->Ticker.initial;
      the_info->start_time = the_timer->Ticker.start_time;
      the_info->stop_time  = the_timer->Ticker.stop_time;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
