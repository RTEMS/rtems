/**
 *  @file
 *
 *  @brief Scheduler Priority Unblock
 *  @ingroup RTEMSScoreScheduler
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Unblock (
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *the_node;
  unsigned int                priority;
  unsigned int                unmapped_priority;

  context = _Scheduler_priority_Get_context( scheduler );
  the_node = _Scheduler_priority_Node_downcast( node );
  priority = (unsigned int ) _Scheduler_Node_get_priority( &the_node->Base );
  unmapped_priority = SCHEDULER_PRIORITY_UNMAP( priority );

  if ( unmapped_priority != the_node->Ready_queue.current_priority ) {
    _Scheduler_priority_Ready_queue_update(
      &the_node->Ready_queue,
      unmapped_priority,
      &context->Bit_map,
      &context->Ready[ 0 ]
    );
  }

  _Scheduler_priority_Ready_queue_enqueue(
    &the_thread->Object.Node,
    &the_node->Ready_queue,
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
  if ( priority < _Thread_Get_priority( _Thread_Heir ) ) {
    _Scheduler_Update_heir( the_thread, priority == PRIORITY_PSEUDO_ISR );
  }
}
