/*
 *  Rate Monotonic Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  rtems_rate_monotonic_create
 *
 *  This directive creates a rate monotonic timer and performs
 *  some initialization.
 *
 *  Input parameters:
 *    name - name of period
 *    id   - pointer to rate monotonic id
 *
 *  Output parameters:
 *    id                - rate monotonic id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_rate_monotonic_create(
  rtems_name    name,
  Objects_Id   *id
)
{
  Rate_monotonic_Control *the_period;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  _Thread_Disable_dispatch();            /* to prevent deletion */

  the_period = _Rate_monotonic_Allocate();

  if ( !the_period ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_period->owner = _Thread_Executing;
  the_period->state = RATE_MONOTONIC_INACTIVE;

  _Objects_Open(
    &_Rate_monotonic_Information,
    &the_period->Object,
    (Objects_Name) name
  );

  *id = the_period->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
