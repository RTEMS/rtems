/**
 *  @file
 *
 *  @brief Thread Queue Initialize
 *  @ingroup ScoreThreadQ
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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/threadimpl.h>

RBTree_Compare_result _Thread_queue_Compare_priority(
  const RBTree_Node *left,
  const RBTree_Node *right
)
{
  Priority_Control left_priority =
    THREAD_RBTREE_NODE_TO_THREAD( left )->current_priority;
  Priority_Control right_priority =
    THREAD_RBTREE_NODE_TO_THREAD( right )->current_priority;

  /*
   * SuperCore priorities use lower numbers to indicate greater importance.
   */
  if ( left_priority == right_priority )
    return 0;
  if ( left_priority < right_priority )
    return -1;
  return 1;
}

void _Thread_queue_Initialize(
  Thread_queue_Control         *the_thread_queue,
  Thread_queue_Disciplines      the_discipline,
  States_Control                state,
  uint32_t                      timeout_status
)
{
  if ( _Scheduler_FIXME_thread_priority_queues_are_broken ) {
    the_discipline = THREAD_QUEUE_DISCIPLINE_FIFO;
  }

  the_thread_queue->state          = state;
  the_thread_queue->discipline     = the_discipline;
  the_thread_queue->timeout_status = timeout_status;
  the_thread_queue->sync_state     = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;

  if ( the_discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY ) {
    _RBTree_Initialize_empty( &the_thread_queue->Queues.Priority );
  } else { /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    _Chain_Initialize_empty( &the_thread_queue->Queues.Fifo );
  }

}
