/**
 *  @file
 *
 *  @brief Thread Queue First
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
)
{
  ISR_Level       level;
  Thread_Control *thread;

  thread = NULL;

  _ISR_Disable( level );

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_FIFO ) {
    if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) )
      thread = (Thread_Control *) _Chain_First(&the_thread_queue->Queues.Fifo);
  } else { /* must be THREAD_QUEUE_DISCIPLINE_PRIORITY */
    RBTree_Node *first;

    first = _RBTree_First( &the_thread_queue->Queues.Priority, RBT_LEFT );
    if ( first )
      thread = THREAD_RBTREE_NODE_TO_THREAD( first );
  }

  _ISR_Enable( level );

  return thread;
}
