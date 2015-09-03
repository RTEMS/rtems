/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/schedulerimpl.h>

static void _Thread_queue_Do_nothing_priority_change(
  Thread_Control     *the_thread,
  Priority_Control    new_priority,
  Thread_queue_Queue *queue
)
{
  /* Do nothing */
}

static void _Thread_queue_Do_nothing_extract(
  Thread_queue_Queue *queue,
  Thread_Control    *the_thread
)
{
  /* Do nothing */
}

static void _Thread_queue_Queue_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  void             ( *initialize )( Thread_queue_Heads * ),
  void             ( *enqueue )( Thread_queue_Heads *, Thread_Control * )
)
{
  Thread_queue_Heads *heads = queue->heads;
  Thread_queue_Heads *spare_heads = the_thread->Wait.spare_heads;

  the_thread->Wait.spare_heads = NULL;

  if ( heads == NULL ) {
    _Assert( spare_heads != NULL );
    _Assert( _Chain_Is_empty( &spare_heads->Free_chain ) );
    heads = spare_heads;
    queue->heads = heads;
    ( *initialize )( heads );
  }

  _Chain_Prepend_unprotected( &heads->Free_chain, &spare_heads->Free_node );

  ( *enqueue )( heads, the_thread );
}

static void _Thread_queue_Queue_extract(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  void             ( *extract )( Thread_queue_Heads *, Thread_Control * )
)
{
  Thread_queue_Heads *heads = queue->heads;

  _Assert( heads != NULL );

  the_thread->Wait.spare_heads = RTEMS_CONTAINER_OF(
    _Chain_Get_first_unprotected( &heads->Free_chain ),
    Thread_queue_Heads,
    Free_node
  );

  if ( _Chain_Is_empty( &heads->Free_chain ) ) {
    queue->heads = NULL;
  }

  ( *extract )( heads, the_thread );
}

static void _Thread_queue_FIFO_do_initialize(
  Thread_queue_Heads *heads
)
{
  _Chain_Initialize_empty( &heads->Heads.Fifo );
}

static void _Thread_queue_FIFO_do_enqueue(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  _Chain_Append_unprotected(
    &heads->Heads.Fifo,
    &the_thread->Wait.Node.Chain
  );
}

static void _Thread_queue_FIFO_do_extract(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  _Chain_Extract_unprotected( &the_thread->Wait.Node.Chain );
}

static void _Thread_queue_FIFO_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    _Thread_queue_FIFO_do_initialize,
    _Thread_queue_FIFO_do_enqueue
  );
}

static void _Thread_queue_FIFO_extract(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  _Thread_queue_Queue_extract(
    queue,
    the_thread,
    _Thread_queue_FIFO_do_extract
  );
}

static Thread_Control *_Thread_queue_FIFO_first(
  Thread_queue_Heads *heads
)
{
  Chain_Control *fifo = &heads->Heads.Fifo;
  Chain_Node    *first;

  _Assert( !_Chain_Is_empty( fifo ) );
  first = _Chain_First( fifo );

  return THREAD_CHAIN_NODE_TO_THREAD( first );
}

static Thread_queue_Priority_queue *_Thread_queue_Priority_queue(
  Thread_queue_Heads   *heads,
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return &heads->Priority[
    _Scheduler_Get_index( _Scheduler_Get_own( the_thread ) )
  ];
#else
  (void) the_thread;

  return &heads->Heads.Priority;
#endif
}

static void _Thread_queue_Priority_priority_change(
  Thread_Control     *the_thread,
  Priority_Control    new_priority,
  Thread_queue_Queue *queue
)
{
  Thread_queue_Heads          *heads = queue->heads;
  Thread_queue_Priority_queue *priority_queue;

  _Assert( heads != NULL );

  priority_queue = _Thread_queue_Priority_queue( heads, the_thread );

  _RBTree_Extract(
    &priority_queue->Queue,
    &the_thread->Wait.Node.RBTree
  );
  _RBTree_Insert(
    &priority_queue->Queue,
    &the_thread->Wait.Node.RBTree,
    _Thread_queue_Compare_priority,
    false
  );
}

static void _Thread_queue_Priority_do_initialize(
  Thread_queue_Heads *heads
)
{
#if defined(RTEMS_SMP)
  _Chain_Initialize_empty( &heads->Heads.Fifo );
#else
  _RBTree_Initialize_empty( &heads->Heads.Priority );
#endif
}

static void _Thread_queue_Priority_do_enqueue(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Thread_queue_Priority_queue *priority_queue =
    _Thread_queue_Priority_queue( heads, the_thread );

#if defined(RTEMS_SMP)
  if ( _RBTree_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }
#endif

  _RBTree_Insert(
    &priority_queue->Queue,
    &the_thread->Wait.Node.RBTree,
    _Thread_queue_Compare_priority,
    false
  );
}

static void _Thread_queue_Priority_do_extract(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Thread_queue_Priority_queue *priority_queue =
    _Thread_queue_Priority_queue( heads, the_thread );

  _RBTree_Extract(
    &priority_queue->Queue,
    &the_thread->Wait.Node.RBTree
  );

#if defined(RTEMS_SMP)
  _Chain_Extract_unprotected( &priority_queue->Node );

  if ( !_RBTree_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }
#endif
}

static void _Thread_queue_Priority_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    _Thread_queue_Priority_do_initialize,
    _Thread_queue_Priority_do_enqueue
  );
}

static void _Thread_queue_Priority_extract(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  _Thread_queue_Queue_extract(
    queue,
    the_thread,
    _Thread_queue_Priority_do_extract
  );
}

static Thread_Control *_Thread_queue_Priority_first(
  Thread_queue_Heads *heads
)
{
  Thread_queue_Priority_queue *priority_queue;
  RBTree_Node                 *first;

#if defined(RTEMS_SMP)
  _Assert( !_Chain_Is_empty( &heads->Heads.Fifo ) );
  priority_queue = (Thread_queue_Priority_queue *)
    _Chain_First( &heads->Heads.Fifo );
#else
  priority_queue = &heads->Heads.Priority;
#endif

  _Assert( !_RBTree_Is_empty( &priority_queue->Queue ) );
  first = _RBTree_Minimum( &priority_queue->Queue );

  return THREAD_RBTREE_NODE_TO_THREAD( first );
}

#if defined(RTEMS_SMP)
void _Thread_queue_Boost_priority(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  Thread_queue_Heads *heads = queue->heads;

  if (
    heads != NULL
      && (
        !_Chain_Has_only_one_node( &heads->Heads.Fifo )
          || _RBTree_Is_empty(
            &_Thread_queue_Priority_queue( heads, the_thread )->Queue
          )
      )
  ) {
    _Thread_Raise_priority( the_thread, PRIORITY_PSEUDO_ISR );
  }
}
#endif

const Thread_queue_Operations _Thread_queue_Operations_default = {
  .priority_change = _Thread_queue_Do_nothing_priority_change,
  .extract = _Thread_queue_Do_nothing_extract
  /*
   * The default operations are only used in _Thread_Change_priority() and
   * _Thread_Timeout() and don't have a thread queue associated with them, so
   * the enqueue and first operations are superfluous.
   */
};

const Thread_queue_Operations _Thread_queue_Operations_FIFO = {
  .priority_change = _Thread_queue_Do_nothing_priority_change,
  .enqueue = _Thread_queue_FIFO_enqueue,
  .extract = _Thread_queue_FIFO_extract,
  .first = _Thread_queue_FIFO_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority = {
  .priority_change = _Thread_queue_Priority_priority_change,
  .enqueue = _Thread_queue_Priority_enqueue,
  .extract = _Thread_queue_Priority_extract,
  .first = _Thread_queue_Priority_first
};
