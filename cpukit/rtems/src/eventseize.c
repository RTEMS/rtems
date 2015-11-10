/**
 * @file
 *
 * @brief Event Manager Initialization
 * @ingroup ClassicEvent Events
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

/*
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 *    check sync
 */

void _Event_Seize(
  rtems_event_set    event_in,
  rtems_option       option_set,
  rtems_interval     ticks,
  rtems_event_set   *event_out,
  Thread_Control    *executing,
  Event_Control     *event,
  Thread_Wait_flags  wait_class,
  States_Control     block_state,
  ISR_lock_Context  *lock_context
)
{
  rtems_event_set    seized_events;
  rtems_event_set    pending_events;
  bool               success;
  Thread_Wait_flags  intend_to_block;
  Per_CPU_Control   *cpu_self;

  executing->Wait.return_code = RTEMS_SUCCESSFUL;

  pending_events = event->pending_events;
  seized_events  = _Event_sets_Get( pending_events, event_in );

  if ( !_Event_sets_Is_empty( seized_events ) &&
       (seized_events == event_in || _Options_Is_any( option_set )) ) {
    event->pending_events =
      _Event_sets_Clear( pending_events, seized_events );
    _Thread_Lock_release_default( executing, lock_context );
    *event_out = seized_events;
    return;
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _Thread_Lock_release_default( executing, lock_context );
    executing->Wait.return_code = RTEMS_UNSATISFIED;
    *event_out = seized_events;
    return;
  }

  intend_to_block = wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK;

  /*
   *  Note what we are waiting for BEFORE we enter the critical section.
   *  The interrupt critical section management code needs this to be
   *  set properly when we are marked as in the event critical section.
   *
   *  NOTE: Since interrupts are disabled, this isn't that much of an
   *        issue but better safe than sorry.
   */
  executing->Wait.option          = option_set;
  executing->Wait.count           = event_in;
  executing->Wait.return_argument = event_out;
  _Thread_Wait_flags_set( executing, intend_to_block );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Thread_Lock_release_default( executing, lock_context );

  if ( ticks ) {
    _Thread_Wait_set_timeout_code( executing, RTEMS_TIMEOUT );
    _Watchdog_Initialize(
      &executing->Timer,
      _Thread_Timeout,
      0,
      executing
    );
    _Watchdog_Insert_ticks( &executing->Timer, ticks );
  }

  _Thread_Set_state( executing, block_state );

  /*
   * See _Event_Surrender() and _Thread_Timeout(), corresponding atomic
   * variable is Thread_Control::Wait::flags.
   */
  _Atomic_Fence( ATOMIC_ORDER_ACQUIRE );

  success = _Thread_Wait_flags_try_change(
    executing,
    intend_to_block,
    wait_class | THREAD_WAIT_STATE_BLOCKED
  );
  if ( !success ) {
    _Watchdog_Remove_ticks( &executing->Timer );
    _Thread_Unblock( executing );
  }

  _Thread_Dispatch_enable( cpu_self );
}
