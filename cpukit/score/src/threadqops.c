/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  Priority_Control    new_priority
)
{
  (void) queue;
  (void) the_thread;
  (void) new_priority;
}

static void _Thread_queue_Do_nothing_extract(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  (void) queue;
  (void) the_thread;
}

static Thread_queue_Heads *_Thread_queue_Queue_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  void             ( *initialize )( Thread_queue_Heads *, Thread_Control * ),
  void             ( *enqueue )( Thread_queue_Heads *, Thread_Control * )
)
{
  Thread_queue_Heads *heads;
  Thread_queue_Heads *spare_heads;

  heads = queue->heads;
  spare_heads = the_thread->Wait.spare_heads;
  the_thread->Wait.spare_heads = NULL;

  if ( heads == NULL ) {
    _Assert( spare_heads != NULL );
    _Assert( _Chain_Is_empty( &spare_heads->Free_chain ) );

    heads = spare_heads;
    queue->heads = heads;
    _Chain_Prepend_unprotected( &heads->Free_chain, &spare_heads->Free_node );
    ( *initialize )( heads, the_thread );
  } else {
    _Chain_Prepend_unprotected( &heads->Free_chain, &spare_heads->Free_node );
    ( *enqueue )( heads, the_thread );
  }

  return heads;
}

static void _Thread_queue_Queue_extract(
  Thread_queue_Queue *queue,
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread,
  void             ( *extract )( Thread_queue_Heads *, Thread_Control * )
)
{
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
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );

  _Chain_Initialize_node( &scheduler_node->Wait.Node.Chain );
  _Chain_Initialize_one(
    &heads->Heads.Fifo,
    &scheduler_node->Wait.Node.Chain
  );
}

static void _Thread_queue_FIFO_do_enqueue(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );

  _Chain_Initialize_node( &scheduler_node->Wait.Node.Chain );
  _Chain_Append_unprotected(
    &heads->Heads.Fifo,
    &scheduler_node->Wait.Node.Chain
  );
}

static void _Thread_queue_FIFO_do_extract(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );
  _Chain_Extract_unprotected( &scheduler_node->Wait.Node.Chain );
}

static void _Thread_queue_FIFO_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  Thread_queue_Path  *path
)
{
  path->update_priority = NULL;

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
    queue->heads,
    the_thread,
    _Thread_queue_FIFO_do_extract
  );
}

static Thread_Control *_Thread_queue_FIFO_first(
  Thread_queue_Heads *heads
)
{
  Chain_Control  *fifo;
  Chain_Node     *first;
  Scheduler_Node *scheduler_node;

  fifo = &heads->Heads.Fifo;
  _Assert( !_Chain_Is_empty( fifo ) );
  first = _Chain_First( fifo );
  scheduler_node = SCHEDULER_NODE_OF_WAIT_CHAIN_NODE( first );

  return _Scheduler_Node_get_owner( scheduler_node );
}

static Thread_Control *_Thread_queue_FIFO_surrender(
  Thread_queue_Queue *queue,
  Thread_queue_Heads *heads,
  Thread_Control     *previous_owner
)
{
  Thread_Control *first;

  first = _Thread_queue_FIFO_first( heads );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    first,
    _Thread_queue_FIFO_do_extract
  );

  return first;
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

static bool _Thread_queue_Priority_less(
  const void        *left,
  const RBTree_Node *right
)
{
  const Priority_Control *the_left;
  const Scheduler_Node   *scheduler_node;
  const Thread_Control   *the_right;

  the_left = left;
  scheduler_node = SCHEDULER_NODE_OF_WAIT_RBTREE_NODE( right );
  the_right = _Scheduler_Node_get_owner( scheduler_node );

  return *the_left < the_right->current_priority;
}

static void _Thread_queue_Priority_priority_change(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  Priority_Control    new_priority
)
{
  Thread_queue_Heads          *heads;
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node;

  heads = queue->heads;
  _Assert( heads != NULL );

  priority_queue = _Thread_queue_Priority_queue( heads, the_thread );
  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );

  _RBTree_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Node.RBTree
  );
  _RBTree_Insert_inline(
    &priority_queue->Queue,
    &scheduler_node->Wait.Node.RBTree,
    &new_priority,
    _Thread_queue_Priority_less
  );
}

static void _Thread_queue_Priority_do_initialize(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node;

  priority_queue = _Thread_queue_Priority_queue( heads, the_thread );

#if defined(RTEMS_SMP)
  _Chain_Initialize_one( &heads->Heads.Fifo, &priority_queue->Node );
#endif

  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );

  _RBTree_Initialize_node( &scheduler_node->Wait.Node.RBTree );
  _RBTree_Initialize_one(
    &priority_queue->Queue,
    &scheduler_node->Wait.Node.RBTree
  );
}

static void _Thread_queue_Priority_do_enqueue(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node;
  Priority_Control             current_priority;

  priority_queue = _Thread_queue_Priority_queue( heads, the_thread );

#if defined(RTEMS_SMP)
  if ( _RBTree_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }
#endif

  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );
  current_priority = the_thread->current_priority;

  _RBTree_Initialize_node( &scheduler_node->Wait.Node.RBTree );
  _RBTree_Insert_inline(
    &priority_queue->Queue,
    &scheduler_node->Wait.Node.RBTree,
    &current_priority,
    _Thread_queue_Priority_less
  );
}

static void _Thread_queue_Priority_do_extract(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node;

  priority_queue = _Thread_queue_Priority_queue( heads, the_thread );
  scheduler_node = _Scheduler_Thread_get_own_node( the_thread );

  _RBTree_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Node.RBTree
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
  Thread_Control     *the_thread,
  Thread_queue_Path  *path
)
{
  path->update_priority = NULL;

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
    queue->heads,
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
  Scheduler_Node              *scheduler_node;

#if defined(RTEMS_SMP)
  _Assert( !_Chain_Is_empty( &heads->Heads.Fifo ) );
  priority_queue = (Thread_queue_Priority_queue *)
    _Chain_First( &heads->Heads.Fifo );
#else
  priority_queue = &heads->Heads.Priority;
#endif

  _Assert( !_RBTree_Is_empty( &priority_queue->Queue ) );
  first = _RBTree_Minimum( &priority_queue->Queue );
  scheduler_node = SCHEDULER_NODE_OF_WAIT_RBTREE_NODE( first );

  return _Scheduler_Node_get_owner( scheduler_node );
}

static Thread_Control *_Thread_queue_Priority_surrender(
  Thread_queue_Queue *queue,
  Thread_queue_Heads *heads,
  Thread_Control     *previous_owner
)
{
  Thread_Control *first;

  first = _Thread_queue_Priority_first( heads );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    first,
    _Thread_queue_Priority_do_extract
  );

  return first;
}

static void _Thread_queue_Priority_inherit_enqueue(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread,
  Thread_queue_Path  *path
)
{
  Thread_queue_Heads *heads;
  Thread_Control     *owner;
  Priority_Control    priority;

  heads = _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    _Thread_queue_Priority_do_initialize,
    _Thread_queue_Priority_do_enqueue
  );

  owner = queue->owner;

#if defined(RTEMS_SMP)
  if ( _Chain_Has_only_one_node( &heads->Heads.Fifo ) ) {
    priority = the_thread->current_priority;
  } else {
    priority = _Scheduler_Map_priority(
      _Scheduler_Get_own( the_thread ),
      PRIORITY_PSEUDO_ISR
    );
  }
#else
  (void) heads;

  priority = the_thread->current_priority;
#endif

  if ( priority < owner->current_priority ) {
    path->update_priority = owner;

    owner->priority_restore_hint = true;
    _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

    _Scheduler_Thread_set_priority( owner, priority, false );

    ( *owner->Wait.operations->priority_change )(
      owner->Wait.queue,
      owner,
      priority
    );
  } else {
    path->update_priority = NULL;
  }
}

static void _Thread_queue_Boost_priority(
  Thread_queue_Heads *heads,
  Thread_Control     *the_thread
)
{
#if defined(RTEMS_SMP)
  if ( !_Chain_Has_only_one_node( &heads->Heads.Fifo ) ) {
    const Scheduler_Control *scheduler;
    Priority_Control         boost_priority;

    the_thread->priority_restore_hint = true;
    _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

    scheduler = _Scheduler_Get_own( the_thread );
    boost_priority = _Scheduler_Map_priority( scheduler, PRIORITY_PSEUDO_ISR );

    _Scheduler_Thread_set_priority( the_thread, boost_priority, false );
  }
#else
  (void) heads;
  (void) the_thread;
#endif
}

static Thread_Control *_Thread_queue_Priority_inherit_surrender(
  Thread_queue_Queue *queue,
  Thread_queue_Heads *heads,
  Thread_Control     *previous_owner
)
{
  Thread_Control *first;

  first = _Thread_queue_Priority_first( heads );
  _Thread_queue_Boost_priority( heads, first );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    first,
    _Thread_queue_Priority_do_extract
  );

  return first;
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
  .surrender = _Thread_queue_FIFO_surrender,
  .first = _Thread_queue_FIFO_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority = {
  .priority_change = _Thread_queue_Priority_priority_change,
  .enqueue = _Thread_queue_Priority_enqueue,
  .extract = _Thread_queue_Priority_extract,
  .surrender = _Thread_queue_Priority_surrender,
  .first = _Thread_queue_Priority_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority_inherit = {
  .priority_change = _Thread_queue_Priority_priority_change,
  .enqueue = _Thread_queue_Priority_inherit_enqueue,
  .extract = _Thread_queue_Priority_extract,
  .surrender = _Thread_queue_Priority_inherit_surrender,
  .first = _Thread_queue_Priority_first
};
