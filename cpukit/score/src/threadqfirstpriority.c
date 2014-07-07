/**
 * @file
 *
 * @brief Returns Highest Priority Thread on Thread Queue
 * @ingroup ScoreThreadQ
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
#include <rtems/score/rbtreeimpl.h>

Thread_Control *_Thread_queue_First_priority (
  Thread_queue_Control *the_thread_queue
)
{
  RBTree_Node *first;

  first = _RBTree_First( &the_thread_queue->Queues.Priority, RBT_LEFT );
  if ( first )
    return _RBTree_Container_of( first, Thread_Control, RBNode );
  return NULL;
}
