/**
 * @file
 *
 * @brief Changes the Priority of a Thread
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

void _Thread_Change_priority(
  Thread_Control                *the_thread,
  Priority_Control               new_priority,
  void                          *arg,
  Thread_Change_priority_filter  filter,
  bool                           prepend_it
)
{
  ISR_lock_Context  lock_context;
  ISR_lock_Control *lock;

  lock = _Thread_Lock_acquire( the_thread, &lock_context );

  /*
   * For simplicity set the priority restore hint unconditionally since this is
   * an average case optimization.  Otherwise complicated atomic operations
   * would be necessary.  Synchronize with a potential read of the resource
   * count in the filter function.  See also _CORE_mutex_Surrender(),
   * _Thread_Set_priority_filter() and _Thread_Restore_priority_filter().
   */
  the_thread->priority_restore_hint = true;
  _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

  /*
   *  Do not bother recomputing all the priority related information if
   *  we are not REALLY changing priority.
   */
  if ( ( *filter )( the_thread, &new_priority, arg ) ) {
    uint32_t my_generation;

    my_generation = the_thread->priority_generation + 1;
    the_thread->current_priority = new_priority;
    the_thread->priority_generation = my_generation;

    ( *the_thread->Wait.operations->priority_change )(
      the_thread,
      new_priority,
      the_thread->Wait.queue
    );

    _Thread_Lock_release( lock, &lock_context );

    _Scheduler_Acquire( the_thread, &lock_context );

    if ( the_thread->priority_generation == my_generation ) {
      if ( _States_Is_ready( the_thread->current_state ) ) {
        _Scheduler_Change_priority(
          the_thread,
          new_priority,
          prepend_it
        );
      } else {
        _Scheduler_Update_priority( the_thread, new_priority );
      }
    }

    _Scheduler_Release( the_thread, &lock_context );
  } else {
    _Thread_Lock_release( lock, &lock_context );
  }
}

static bool _Thread_Raise_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  return _Thread_Priority_less_than(
    the_thread->current_priority,
    *new_priority
  );
}

void _Thread_Raise_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
)
{
  _Thread_Change_priority(
    the_thread,
    new_priority,
    NULL,
    _Thread_Raise_priority_filter,
    false
  );
}

static bool _Thread_Restore_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  *new_priority = the_thread->real_priority;

  the_thread->priority_restore_hint = false;

  return *new_priority != the_thread->current_priority;
}

void _Thread_Restore_priority( Thread_Control *the_thread )
{
  _Thread_Change_priority(
    the_thread,
    0,
    NULL,
    _Thread_Restore_priority_filter,
    true
  );
}
