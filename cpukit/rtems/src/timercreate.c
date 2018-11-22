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

#include <rtems/rtems/timerimpl.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/sysinit.h>

RTEMS_STATIC_ASSERT(
  PER_CPU_WATCHDOG_REALTIME == TIMER_CLASS_BIT_TIME_OF_DAY,
  TIMER_CLASS_BIT_TIME_OF_DAY
);

Timer_server_Control *volatile _Timer_server;

void _Timer_Routine_adaptor( Watchdog_Control *the_watchdog )
{
  Timer_Control   *the_timer;
  Per_CPU_Control *cpu;

  the_timer = RTEMS_CONTAINER_OF( the_watchdog, Timer_Control, Ticker );
  cpu = _Watchdog_Get_CPU( &the_timer->Ticker );
  the_timer->stop_time = _Timer_Get_CPU_ticks( cpu );

  ( *the_timer->routine )( the_timer->Object.id, the_timer->user_data );
}

rtems_status_code _Timer_Fire(
  rtems_id                           id,
  rtems_interval                     interval,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
)
{
  Timer_Control    *the_timer;
  ISR_lock_Context  lock_context;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    Per_CPU_Control *cpu;

    cpu = _Timer_Acquire_critical( the_timer, &lock_context );
    _Timer_Cancel( cpu, the_timer );
    _Watchdog_Initialize( &the_timer->Ticker, adaptor );
    the_timer->the_class = the_class;
    the_timer->routine = routine;
    the_timer->user_data = user_data;
    the_timer->initial = interval;
    the_timer->start_time = _Timer_Get_CPU_ticks( cpu );

    if ( _Timer_Is_interval_class( the_class ) ) {
      _Watchdog_Insert(
        &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
        &the_timer->Ticker,
        cpu->Watchdog.ticks + interval
      );
    } else {
      _Watchdog_Insert(
        &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ],
        &the_timer->Ticker,
        _Watchdog_Ticks_from_seconds( interval )
      );
    }

    _Timer_Release( cpu, &lock_context );
    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INVALID_ID;
}

rtems_status_code _Timer_Fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
)
{
  if ( ticks == 0 )
    return RTEMS_INVALID_NUMBER;

  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  return _Timer_Fire(
    id,
    ticks,
    routine,
    user_data,
    the_class,
    adaptor
  );
}

rtems_status_code _Timer_Fire_when(
  rtems_id                           id,
  const rtems_time_of_day           *wall_time,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
)
{
  rtems_interval seconds;

  if ( !_TOD_Is_set() )
    return RTEMS_NOT_DEFINED;

  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  if ( !_TOD_Validate( wall_time ) )
    return RTEMS_INVALID_CLOCK;

  seconds = _TOD_To_seconds( wall_time );
  if ( seconds <= _TOD_Seconds_since_epoch() )
    return RTEMS_INVALID_CLOCK;

  return _Timer_Fire(
    id,
    seconds,
    routine,
    user_data,
    the_class,
    adaptor
  );
}

void _Timer_Cancel( Per_CPU_Control *cpu, Timer_Control *the_timer )
{
  Timer_Classes the_class;

  the_class = the_timer->the_class;

  if ( _Watchdog_Is_scheduled( &the_timer->Ticker ) ) {
    the_timer->stop_time = _Timer_Get_CPU_ticks( cpu );
    _Watchdog_Remove(
      &cpu->Watchdog.Header[ _Timer_Watchdog_header_index( the_class ) ],
      &the_timer->Ticker
    );
  } else if ( _Timer_Is_on_task_class( the_class ) ) {
    Timer_server_Control *timer_server;
    ISR_lock_Context      lock_context;

    timer_server = _Timer_server;
    _Assert( timer_server != NULL );
    _Timer_server_Acquire_critical( timer_server, &lock_context );

    if ( _Watchdog_Get_state( &the_timer->Ticker ) == WATCHDOG_PENDING ) {
      _Watchdog_Set_state( &the_timer->Ticker, WATCHDOG_INACTIVE );
      _Chain_Extract_unprotected( &the_timer->Ticker.Node.Chain );
    }

    _Timer_server_Release_critical( timer_server, &lock_context );
  }
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
  _Watchdog_Preinitialize( &the_timer->Ticker, _Per_CPU_Get_snapshot() );

  _Objects_Open(
    &_Timer_Information,
    &the_timer->Object,
    (Objects_Name) name
  );

  *id = the_timer->Object.id;
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Timer_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Timer_Information );
}

RTEMS_SYSINIT_ITEM(
  _Timer_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_TIMER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
