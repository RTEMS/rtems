/**
 * @file
 *
 * @brief Scheduler EDF Release Job
 * @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

static bool _Scheduler_EDF_Release_job_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_p,
  void             *arg
)
{
  Scheduler_EDF_Node *node;
  Priority_Control    current_priority;
  Priority_Control    new_priority;

  node = _Scheduler_EDF_Thread_get_node( the_thread );

  current_priority = the_thread->current_priority;
  new_priority = *new_priority_p;

  node->current_priority = new_priority;
  the_thread->real_priority = new_priority;

  return _Thread_Priority_less_than( current_priority, new_priority )
    || !_Thread_Owns_resources( the_thread );
}

void _Scheduler_EDF_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  uint64_t                 deadline
)
{
  _Thread_Change_priority(
    the_thread,
    deadline,
    NULL,
    _Scheduler_EDF_Release_job_filter,
    true
  );
}

static bool _Scheduler_EDF_Cancel_job_filter(
  Thread_Control   *the_thread,
  Priority_Control *new_priority_p,
  void             *arg
)
{
  Scheduler_EDF_Node *node;
  Priority_Control    current_priority;
  Priority_Control    new_priority;

  node = _Scheduler_EDF_Thread_get_node( the_thread );

  current_priority = the_thread->current_priority;
  new_priority = node->background_priority;

  node->current_priority = new_priority;
  the_thread->real_priority = new_priority;

  return _Thread_Priority_less_than( current_priority, new_priority )
    || !_Thread_Owns_resources( the_thread );
}

void _Scheduler_EDF_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  _Thread_Change_priority(
    the_thread,
    0,
    NULL,
    _Scheduler_EDF_Cancel_job_filter,
    true
  );
}
