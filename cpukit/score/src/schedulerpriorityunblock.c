/**
 *  @file
 *
 *  @brief Scheduler Priority Unblock
 *  @ingroup ScoreScheduler
 */

/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

Scheduler_Void_or_thread _Scheduler_priority_Unblock (
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *node;
  unsigned int                priority;
  bool                        prepend_it;

  context = _Scheduler_priority_Get_context( scheduler );
  node = _Scheduler_priority_Thread_get_node( the_thread );
  priority = (unsigned int )
    _Scheduler_Node_get_priority( &node->Base, &prepend_it );
  (void) prepend_it;

  if ( priority != node->Ready_queue.current_priority ) {
    _Scheduler_priority_Ready_queue_update(
      &node->Ready_queue,
      priority,
      &context->Bit_map,
      &context->Ready[ 0 ]
    );
  }

  _Scheduler_priority_Ready_queue_enqueue(
    &the_thread->Object.Node,
    &node->Ready_queue,
    &context->Bit_map
  );

  /* TODO: flash critical section? */

  /*
   *  If the thread that was unblocked is more important than the heir,
   *  then we have a new heir.  This may or may not result in a
   *  context switch.
   *
   *  Normal case:
   *    If the current thread is preemptible, then we need to do
   *    a context switch.
   *  Pseudo-ISR case:
   *    Even if the thread isn't preemptible, if the new heir is
   *    a pseudo-ISR system task, we need to do a context switch.
   */
  if ( priority < _Thread_Heir->current_priority ) {
    _Scheduler_Update_heir( the_thread, priority == PRIORITY_PSEUDO_ISR );
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
