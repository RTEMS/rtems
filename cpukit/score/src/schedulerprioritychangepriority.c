/**
 * @file
 *
 * @brief Removes Thread from Thread Queue
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

Scheduler_Void_or_thread _Scheduler_priority_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
)
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );
  Scheduler_priority_Node *node = _Scheduler_priority_Thread_get_node( the_thread );

  _Scheduler_priority_Ready_queue_extract(
    &the_thread->Object.Node,
    &node->Ready_queue,
    &context->Bit_map
  );

  _Scheduler_priority_Ready_queue_update(
    &node->Ready_queue,
    new_priority,
    &context->Bit_map,
    &context->Ready[ 0 ]
  );

  if ( prepend_it ) {
    _Scheduler_priority_Ready_queue_enqueue_first(
      &the_thread->Object.Node,
      &node->Ready_queue,
      &context->Bit_map
    );
  } else {
    _Scheduler_priority_Ready_queue_enqueue(
      &the_thread->Object.Node,
      &node->Ready_queue,
      &context->Bit_map
    );
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
