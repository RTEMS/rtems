/**
 *  @file
 *
 *  @brief RTEMS Create Timer
 *  @ingroup ClassicTimer
 */

/*
 *  COPYRIGHT (c) 1989-2002.
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
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_timer_create(
  rtems_name  name,
  rtems_id   *id
)
{
  Timer_Control *the_timer;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  _Thread_Disable_dispatch();         /* to prevent deletion */

  the_timer = _Timer_Allocate();

  if ( !the_timer ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_timer->the_class = TIMER_DORMANT;
  _Watchdog_Initialize( &the_timer->Ticker, NULL, 0, NULL );

  _Objects_Open(
    &_Timer_Information,
    &the_timer->Object,
    (Objects_Name) name
  );

  *id = the_timer->Object.id;
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
