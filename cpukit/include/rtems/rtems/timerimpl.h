/**
 * @file
 *
 * @ingroup ClassicTimerImpl
 *
 * @brief Classic Timer Implementation
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TIMER_INL
#define _RTEMS_RTEMS_TIMER_INL

#include <rtems/rtems/timerdata.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdogimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicTimerImpl Classic Timer Implementation
 *
 * @ingroup RTEMSInternalClassic
 *
 * @{
 */

typedef struct Timer_server_Control {
  ISR_LOCK_MEMBER( Lock )

  Chain_Control Pending;

  Objects_Id server_id;
} Timer_server_Control;

/**
 * @brief Pointer to default timer server control block.
 *
 * This value is @c NULL when the default timer server is not initialized.
 */
extern Timer_server_Control *volatile _Timer_server;

/**
 *  @brief Timer_Allocate
 *
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Allocate( void )
{
  return (Timer_Control *) _Objects_Allocate( &_Timer_Information );
}

/**
 *  @brief Timer_Free
 *
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 */
RTEMS_INLINE_ROUTINE void _Timer_Free (
  Timer_Control *the_timer
)
{
  _Objects_Free( &_Timer_Information, &the_timer->Object );
}

RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
)
{
  return (Timer_Control *) _Objects_Get(
    id,
    lock_context,
    &_Timer_Information
  );
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Timer_Acquire_critical(
  Timer_Control    *the_timer,
  ISR_lock_Context *lock_context
)
{
  Per_CPU_Control *cpu;

  cpu = _Watchdog_Get_CPU( &the_timer->Ticker );
  _Watchdog_Per_CPU_acquire_critical( cpu, lock_context );

  return cpu;
}

RTEMS_INLINE_ROUTINE void _Timer_Release(
  Per_CPU_Control  *cpu,
  ISR_lock_Context *lock_context
)
{
  _Watchdog_Per_CPU_release_critical( cpu, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

RTEMS_INLINE_ROUTINE bool _Timer_Is_interval_class(
  Timer_Classes the_class
)
{
  Timer_Classes mask =
    TIMER_CLASS_BIT_NOT_DORMANT | TIMER_CLASS_BIT_TIME_OF_DAY;

  return ( the_class & mask ) == TIMER_CLASS_BIT_NOT_DORMANT;
}

RTEMS_INLINE_ROUTINE bool _Timer_Is_on_task_class(
  Timer_Classes the_class
)
{
  Timer_Classes mask =
    TIMER_CLASS_BIT_NOT_DORMANT | TIMER_CLASS_BIT_ON_TASK;

  return ( the_class & mask ) == mask;
}

RTEMS_INLINE_ROUTINE Per_CPU_Watchdog_index _Timer_Watchdog_header_index(
  Timer_Classes the_class
)
{
  return ( the_class & TIMER_CLASS_BIT_TIME_OF_DAY );
}

RTEMS_INLINE_ROUTINE Watchdog_Interval _Timer_Get_CPU_ticks(
  const Per_CPU_Control *cpu
)
{
  return (Watchdog_Interval) cpu->Watchdog.ticks;
}

rtems_status_code _Timer_Fire(
  rtems_id                           id,
  rtems_interval                     interval,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

rtems_status_code _Timer_Fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

rtems_status_code _Timer_Fire_when(
  rtems_id                           id,
  const rtems_time_of_day           *wall_time,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data,
  Timer_Classes                      the_class,
  Watchdog_Service_routine_entry     adaptor
);

void _Timer_Cancel( Per_CPU_Control *cpu, Timer_Control *the_timer );

void _Timer_Routine_adaptor( Watchdog_Control *the_watchdog );

void _Timer_server_Routine_adaptor( Watchdog_Control *the_watchdog );

RTEMS_INLINE_ROUTINE void _Timer_server_Acquire_critical(
  Timer_server_Control *timer_server,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Acquire( &timer_server->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Timer_server_Release_critical(
  Timer_server_Control *timer_server,
  ISR_lock_Context     *lock_context
)
{
  _ISR_lock_Release( &timer_server->Lock, lock_context );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
