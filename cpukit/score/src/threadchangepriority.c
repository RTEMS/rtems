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

static Thread_Control *_Thread_Apply_priority_locked(
  Thread_Control                *the_thread,
  Priority_Control               new_priority,
  void                          *arg,
  Thread_Change_priority_filter  filter,
  bool                           prepend_it,
  Thread_queue_Context          *queue_context
)
{
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
    _Scheduler_Thread_set_priority( the_thread, new_priority, prepend_it );

    ( *the_thread->Wait.operations->priority_change )(
      the_thread->Wait.queue,
      the_thread,
      new_priority
    );
  } else {
    the_thread = NULL;
  }

  return the_thread;
}

Thread_Control *_Thread_Apply_priority(
  Thread_Control                *the_thread,
  Priority_Control               new_priority,
  void                          *arg,
  Thread_Change_priority_filter  filter,
  bool                           prepend_it
)
{
  Thread_queue_Context  queue_context;
  Thread_Control       *the_thread_to_update;

  _Thread_Wait_acquire( the_thread, &queue_context );
  the_thread_to_update = _Thread_Apply_priority_locked(
    the_thread,
    new_priority,
    arg,
    filter,
    prepend_it,
    &queue_context
  );
  _Thread_Wait_release( the_thread, &queue_context );
  return the_thread_to_update;
}

void _Thread_Update_priority( Thread_Control *the_thread )
{
  if ( the_thread != NULL ) {
    ISR_lock_Context lock_context;

    _Thread_State_acquire( the_thread, &lock_context );
    _Scheduler_Update_priority( the_thread );
    _Thread_State_release( the_thread, &lock_context );
  }
}

void _Thread_Change_priority(
  Thread_Control                *the_thread,
  Priority_Control               new_priority,
  void                          *arg,
  Thread_Change_priority_filter  filter,
  bool                           prepend_it
)
{
  the_thread = _Thread_Apply_priority(
    the_thread,
    new_priority,
    arg,
    filter,
    prepend_it
  );
  _Thread_Update_priority( the_thread );
}

static bool _Thread_Raise_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
)
{
  return _Thread_Priority_less_than(
    _Thread_Get_priority( the_thread ),
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

  return *new_priority != _Thread_Get_priority( the_thread );
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
