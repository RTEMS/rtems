/*
 *  Timer Manager - rtems_timer_create directive
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
 *  rtems_timer_create
 *
 *  This directive creates a timer and performs some initialization.
 *
 *  Input parameters:
 *    name - timer name
 *    id   - pointer to timer id
 *
 *  Output parameters:
 *    id               - timer id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_timer_create(
  rtems_name    name,
  Objects_Id   *id
)
{
  Timer_Control *the_timer;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  _Thread_Disable_dispatch();         /* to prevent deletion */

  the_timer = _Timer_Allocate();

  if ( !the_timer ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_timer->the_class = TIMER_DORMANT;

  _Objects_Open( &_Timer_Information, &the_timer->Object, name );

  *id = the_timer->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
