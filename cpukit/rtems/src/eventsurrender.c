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

void _Event_Surrender(
  Thread_Control    *the_thread,
  rtems_event_set    event_in,
  Event_Control     *event,
  Thread_Wait_flags  wait_class,
  ISR_lock_Context  *lock_context
)
{
  rtems_event_set   pending_events;
  rtems_event_set   seized_events;
  Thread_Wait_flags wait_flags;
  bool              unblock;

  _Event_sets_Post( event_in, &event->pending_events );
  pending_events = event->pending_events;

  wait_flags = _Thread_Wait_flags_get( the_thread );

  if (
    ( wait_flags & THREAD_WAIT_CLASS_MASK ) == wait_class
      && _Event_Is_satisfied( the_thread, pending_events, &seized_events )
  ) {
    Thread_Wait_flags intend_to_block;
    Thread_Wait_flags blocked;
    bool success;

    intend_to_block = wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK;
    blocked = wait_class | THREAD_WAIT_STATE_BLOCKED;

    success = _Thread_Wait_flags_try_change_critical(
      the_thread,
      intend_to_block,
      wait_class | THREAD_WAIT_STATE_INTERRUPT_SATISFIED
    );
    if ( success ) {
      _Event_Satisfy( the_thread, event, pending_events, seized_events );
      unblock = false;
    } else if ( _Thread_Wait_flags_get( the_thread ) == blocked ) {
      _Event_Satisfy( the_thread, event, pending_events, seized_events );
      _Thread_Wait_flags_set(
        the_thread,
        wait_class | THREAD_WAIT_STATE_SATISFIED
      );
      unblock = true;
    } else {
      unblock = false;
    }
  } else {
    unblock = false;
  }

  if ( unblock ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Objects_Release_and_thread_dispatch_disable(
      &the_thread->Object,
      lock_context
    );
    _Giant_Acquire( cpu_self );

    _Watchdog_Remove( &the_thread->Timer );
    _Thread_Unblock( the_thread );

    _Giant_Release( cpu_self );
    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Objects_Release_and_ISR_enable( &the_thread->Object, lock_context );
  }
}
