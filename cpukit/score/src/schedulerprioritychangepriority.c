/**
 * @file
 *
 * @brief Removes Thread from Thread Queue
 *
 * @ingroup RTEMSScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *the_node;
  unsigned int                new_priority;
  unsigned int                unmapped_priority;

  if ( !_Thread_Is_ready( the_thread ) ) {
    /* Nothing to do */
    return;
  }

  the_node = _Scheduler_priority_Node_downcast( node );
  new_priority = (unsigned int)
    _Scheduler_Node_get_priority( &the_node->Base );
  unmapped_priority = SCHEDULER_PRIORITY_UNMAP( new_priority );

  if ( unmapped_priority == the_node->Ready_queue.current_priority ) {
    /* Nothing to do */
    return;
  }

  context = _Scheduler_priority_Get_context( scheduler );

  _Scheduler_priority_Ready_queue_extract(
    &the_thread->Object.Node,
    &the_node->Ready_queue,
    &context->Bit_map
  );

  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    unmapped_priority,
    &context->Bit_map,
    &context->Ready[ 0 ]
  );

  if ( SCHEDULER_PRIORITY_IS_APPEND( new_priority ) ) {
    _Scheduler_priority_Ready_queue_enqueue(
      &the_thread->Object.Node,
      &the_node->Ready_queue,
      &context->Bit_map
    );
  } else {
    _Scheduler_priority_Ready_queue_enqueue_first(
      &the_thread->Object.Node,
      &the_node->Ready_queue,
      &context->Bit_map
    );
  }

  _Scheduler_priority_Schedule_body( scheduler, the_thread, false );
}
