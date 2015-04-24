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
#include <rtems/score/chainimpl.h>
#include <rtems/score/rbtreeimpl.h>

static void _Thread_queue_Do_nothing_priority_change(
  Thread_Control       *the_thread,
  Priority_Control      new_priority,
  Thread_queue_Control *the_thread_queue
)
{
  /* Do nothing */
}

static void _Thread_queue_Do_nothing_extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  /* Do nothing */
}

static void _Thread_queue_FIFO_initialize(
  Thread_queue_Control *the_thread_queue
)
{
  _Chain_Initialize_empty( &the_thread_queue->Queues.Fifo );
}

static void _Thread_queue_FIFO_enqueue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _Chain_Append_unprotected(
    &the_thread_queue->Queues.Fifo,
    &the_thread->Wait.Node.Chain
  );
}

static void _Thread_queue_FIFO_extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _Chain_Extract_unprotected( &the_thread->Wait.Node.Chain );
}

static Thread_Control *_Thread_queue_FIFO_first(
  Thread_queue_Control *the_thread_queue
)
{
  Chain_Control *fifo = &the_thread_queue->Queues.Fifo;

  return _Chain_Is_empty( fifo ) ?
    NULL : THREAD_CHAIN_NODE_TO_THREAD( _Chain_First( fifo ) );
}

static void _Thread_queue_Priority_priority_change(
  Thread_Control       *the_thread,
  Priority_Control      new_priority,
  Thread_queue_Control *the_thread_queue
)
{
  _RBTree_Extract(
    &the_thread_queue->Queues.Priority,
    &the_thread->Wait.Node.RBTree
  );
  _RBTree_Insert(
    &the_thread_queue->Queues.Priority,
    &the_thread->Wait.Node.RBTree,
    _Thread_queue_Compare_priority,
    false
  );
}

static void _Thread_queue_Priority_initialize(
  Thread_queue_Control *the_thread_queue
)
{
  _RBTree_Initialize_empty( &the_thread_queue->Queues.Priority );
}

static void _Thread_queue_Priority_enqueue(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _RBTree_Insert(
    &the_thread_queue->Queues.Priority,
    &the_thread->Wait.Node.RBTree,
    _Thread_queue_Compare_priority,
    false
  );
}

static void _Thread_queue_Priority_extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _RBTree_Extract(
    &the_thread_queue->Queues.Priority,
    &the_thread->Wait.Node.RBTree
  );
}

static Thread_Control *_Thread_queue_Priority_first(
  Thread_queue_Control *the_thread_queue
)
{
  RBTree_Node *first;

  first = _RBTree_First( &the_thread_queue->Queues.Priority, RBT_LEFT );

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
  .initialize = _Thread_queue_FIFO_initialize,
  .enqueue = _Thread_queue_FIFO_enqueue,
  .extract = _Thread_queue_FIFO_extract,
  .first = _Thread_queue_FIFO_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority = {
  .priority_change = _Thread_queue_Priority_priority_change,
  .initialize = _Thread_queue_Priority_initialize,
  .enqueue = _Thread_queue_Priority_enqueue,
  .extract = _Thread_queue_Priority_extract,
  .first = _Thread_queue_Priority_first
};
