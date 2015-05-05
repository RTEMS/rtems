/**
 *  @file
 *
 *  @brief Set Thread Priority
 *  @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

static bool _Thread_Set_priority_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_ptr,
  void             *arg
)
{
  Priority_Control  current_priority;
  Priority_Control  new_priority;
  Priority_Control *old_priority_ptr;

  current_priority = the_thread->current_priority;
  new_priority = *new_priority_ptr;

  old_priority_ptr = arg;
  *old_priority_ptr = current_priority;

  the_thread->real_priority = new_priority;

  return _Thread_Priority_less_than( current_priority, new_priority )
    || !_Thread_Owns_resources( the_thread );
}

void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  Priority_Control *old_priority,
  bool              prepend_it
)
{
  _Thread_Change_priority(
    the_thread,
    new_priority,
    old_priority,
    _Thread_Set_priority_filter,
    prepend_it
  );
}
