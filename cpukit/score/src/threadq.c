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
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/threadimpl.h>

#if HAVE_STRUCT__THREAD_QUEUE_QUEUE

RTEMS_STATIC_ASSERT(
  offsetof( Thread_queue_Syslock_queue, Queue.heads )
    == offsetof( struct _Thread_queue_Queue, _heads ),
  THREAD_QUEUE_SYSLOCK_QUEUE_HEADS
);

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( Thread_queue_Syslock_queue, Queue.Lock.next_ticket )
#else
  offsetof( Thread_queue_Syslock_queue, reserved[ 0 ] )
#endif
    == offsetof( struct _Thread_queue_Queue, _Lock._next_ticket ),
  THREAD_QUEUE_SYSLOCK_QUEUE_NEXT_TICKET
);

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( Thread_queue_Syslock_queue, Queue.Lock.now_serving )
#else
  offsetof( Thread_queue_Syslock_queue, reserved[ 1 ] )
#endif
    == offsetof( struct _Thread_queue_Queue, _Lock._now_serving ),
  THREAD_QUEUE_SYSLOCK_QUEUE_NOW_SERVING
);

RTEMS_STATIC_ASSERT(
  sizeof( Thread_queue_Syslock_queue )
    == sizeof( struct _Thread_queue_Queue ),
  THREAD_QUEUE_SYSLOCK_QUEUE_SIZE
);

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */

RBTree_Compare_result _Thread_queue_Compare_priority(
  const RBTree_Node *left,
  const RBTree_Node *right
)
{
  const Thread_Control *left_thread;
  const Thread_Control *right_thread;
  Priority_Control      left_prio;
  Priority_Control      right_prio;

  left_thread = THREAD_RBTREE_NODE_TO_THREAD( left );
  right_thread = THREAD_RBTREE_NODE_TO_THREAD( right );
  left_prio = left_thread->current_priority;
  right_prio = right_thread->current_priority;

  /*
   * SuperCore priorities use lower numbers to indicate greater importance.
   */
  return ( left_prio > right_prio ) - ( left_prio < right_prio );
}

void _Thread_queue_Initialize(
  Thread_queue_Control     *the_thread_queue,
  Thread_queue_Disciplines  the_discipline
)
{
  const Thread_queue_Operations *operations;

  if ( the_discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY ) {
    operations = &_Thread_queue_Operations_priority;
  } else {
    _Assert( the_discipline == THREAD_QUEUE_DISCIPLINE_FIFO );
    operations = &_Thread_queue_Operations_FIFO;
  }

  the_thread_queue->operations = operations;

  _Thread_queue_Queue_initialize( &the_thread_queue->Queue );
#if defined(RTEMS_SMP)
  _SMP_lock_Stats_initialize( &the_thread_queue->Lock_stats, "Thread Queue" );
#endif
}
