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

  return _Chain_Is_empty( fifo ) ?
    NULL : THREAD_CHAIN_NODE_TO_THREAD( _Chain_First( fifo ) );
}

static void _Thread_queue_Priority_priority_change(
  Thread_Control     *the_thread,
  Priority_Control    new_priority,
  Thread_queue_Queue *queue
)
{
  Thread_queue_Heads *heads = queue->heads;

  _Assert( heads != NULL );

  _RBTree_Extract(
    &heads->Heads.Priority,
    &the_thread->Wait.Node.RBTree
  );
  _RBTree_Insert(
    &heads->Heads.Priority,
    &the_thread->Wait.Node.RBTree,
    _Thread_queue_Compare_priority,
    false
  );
}

static void _Thread_queue_Priority_do_initialize(
  Thread_queue_Heads *heads
)
{
  _RBTree_Initialize_empty( &heads->Heads.Priority );
}

static void _Thread_queue_Priority_do_enqueue(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  _RBTree_Insert(
    &heads->Heads.Priority,
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
  _RBTree_Extract(
    &heads->Heads.Priority,
    &the_thread->Wait.Node.RBTree
  );
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
  RBTree_Node *first;

  first = _RBTree_First( &heads->Heads.Priority, RBT_LEFT );

  return first != NULL ? THREAD_RBTREE_NODE_TO_THREAD( first ) : NULL;
}

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
