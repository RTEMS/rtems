/**
 *  @file
 *
 *  @brief Surrender Event
 *  @ingroup ClassicEvent
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

static void _Event_Satisfy(
  Thread_Control  *the_thread,
  Event_Control   *event,
  rtems_event_set  pending_events,
  rtems_event_set  seized_events
)
{
  event->pending_events = _Event_sets_Clear( pending_events, seized_events );
  *(rtems_event_set *) the_thread->Wait.return_argument = seized_events;
}

static bool _Event_Is_blocking_on_event(
  const Thread_Control *the_thread,
  Thread_Wait_flags     wait_class
)
{
  Thread_Wait_flags wait_flags;
  Thread_Wait_flags wait_mask;

  wait_flags = _Thread_Wait_flags_get( the_thread );
  wait_mask = THREAD_WAIT_CLASS_MASK | THREAD_WAIT_STATE_READY_AGAIN;

  return ( wait_flags & wait_mask ) == wait_class;
}

static bool _Event_Is_satisfied(
  const Thread_Control *the_thread,
  rtems_event_set       pending_events,
  rtems_event_set      *seized_events
)
{
  rtems_option    option_set;
  rtems_event_set event_condition;

  option_set = the_thread->Wait.option;
  event_condition = the_thread->Wait.count;
  *seized_events = _Event_sets_Get( pending_events, event_condition );

  return !_Event_sets_Is_empty( *seized_events )
    && ( *seized_events == event_condition || _Options_Is_any( option_set ) );
}

rtems_status_code _Event_Surrender(
  Thread_Control    *the_thread,
  rtems_event_set    event_in,
  Event_Control     *event,
  Thread_Wait_flags  wait_class,
  ISR_lock_Context  *lock_context
)
{
  rtems_event_set pending_events;
  rtems_event_set seized_events;
  bool            unblock;

  _Thread_Wait_acquire_default_critical( the_thread, lock_context );

  _Event_sets_Post( event_in, &event->pending_events );
  pending_events = event->pending_events;

  if (
    _Event_Is_blocking_on_event( the_thread, wait_class )
      && _Event_Is_satisfied( the_thread, pending_events, &seized_events )
  ) {
    Thread_Wait_flags ready_again;
    bool              success;

    _Event_Satisfy( the_thread, event, pending_events, seized_events );

    ready_again = wait_class | THREAD_WAIT_STATE_READY_AGAIN;
    success = _Thread_Wait_flags_try_change_release(
      the_thread,
      wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK,
      ready_again
    );

    if ( success ) {
      unblock = false;
    } else {
      _Assert(
        _Thread_Wait_flags_get( the_thread )
          == ( wait_class | THREAD_WAIT_STATE_BLOCKED )
      );
      _Thread_Wait_flags_set( the_thread, ready_again );
      unblock = true;
    }
  } else {
    unblock = false;
  }

  if ( unblock ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Thread_Wait_release_default( the_thread, lock_context );

    _Thread_Timer_remove( the_thread );
    _Thread_Unblock( the_thread );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_Wait_release_default( the_thread, lock_context );
  }

  return RTEMS_SUCCESSFUL;
}
