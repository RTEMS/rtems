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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/assert.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Timer_Cancel( Timer_Control *the_timer )
{
  Timer_server_Control *timer_server;
  ISR_Level level;

  /* The timer class must not change during the cancel operation */
  _ISR_Disable( level );

  switch ( the_timer->the_class ) {
    case TIMER_INTERVAL:
      _Watchdog_Remove_ticks( &the_timer->Ticker );
      break;
    case TIMER_TIME_OF_DAY:
      _Watchdog_Remove_seconds( &the_timer->Ticker );
      break;
    case TIMER_INTERVAL_ON_TASK:
    case TIMER_TIME_OF_DAY_ON_TASK:
      timer_server = _Timer_server;
      (*timer_server->cancel)( timer_server, the_timer );
      break;
    default:
      _Assert( the_timer->the_class == TIMER_DORMANT );
      break;
  }

  _ISR_Enable( level );
}

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

  the_timer = _Timer_Allocate();

  if ( !the_timer ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  the_timer->the_class = TIMER_DORMANT;
  _Watchdog_Preinitialize( &the_timer->Ticker );

  _Objects_Open(
    &_Timer_Information,
    &the_timer->Object,
    (Objects_Name) name
  );

  *id = the_timer->Object.id;
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
