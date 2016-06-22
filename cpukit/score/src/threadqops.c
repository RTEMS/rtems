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

#define THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions ) \
  RTEMS_CONTAINER_OF( \
    priority_actions, \
    Thread_queue_Context, \
    Priority.Actions \
  )

#define THREAD_QUEUE_PRIORITY_QUEUE_OF_PRIORITY_AGGREGATION( \
  priority_aggregation \
) \
  RTEMS_CONTAINER_OF( \
    priority_aggregation, \
    Thread_queue_Priority_queue, \
    Queue \
  )

static void _Thread_queue_Do_nothing_priority_actions(
  Thread_queue_Queue *queue,
  Priority_Actions   *priority_actions
)
{
  (void) queue;
  _Priority_Actions_initialize_empty( priority_actions );
}

static void _Thread_queue_Do_nothing_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  (void) queue;
  (void) the_thread;
  (void) queue_context;
}

static void _Thread_queue_Queue_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  void               ( *initialize )(
    Thread_queue_Queue *,
    Thread_Control *,
    Thread_queue_Context *,
    Thread_queue_Heads *
  ),
  void               ( *enqueue )(
    Thread_queue_Queue *,
    Thread_Control *,
    Thread_queue_Context *,
    Thread_queue_Heads *
  )
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
    ( *initialize )( queue, the_thread, queue_context, heads );
  } else {
    _Chain_Prepend_unprotected( &heads->Free_chain, &spare_heads->Free_node );
    ( *enqueue )( queue, the_thread, queue_context, heads );
  }
}

static void _Thread_queue_Queue_extract(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *current_or_previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread,
  void               ( *extract )(
    Thread_queue_Queue *,
    Thread_queue_Heads *,
    Thread_Control *,
    Thread_queue_Context *,
    Thread_Control *
  )
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

  ( *extract )(
    queue,
    heads,
    current_or_previous_owner,
    queue_context,
    the_thread
  );
}

static void _Thread_queue_FIFO_do_initialize(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );

  _Chain_Initialize_node( &scheduler_node->Wait.Priority.Node.Node.Chain );
  _Chain_Initialize_one(
    &heads->Heads.Fifo,
    &scheduler_node->Wait.Priority.Node.Node.Chain
  );
}

static void _Thread_queue_FIFO_do_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );

  _Chain_Initialize_node( &scheduler_node->Wait.Priority.Node.Node.Chain );
  _Chain_Append_unprotected(
    &heads->Heads.Fifo,
    &scheduler_node->Wait.Priority.Node.Node.Chain
  );
}

static void _Thread_queue_FIFO_do_extract(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *current_or_previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  Scheduler_Node *scheduler_node;

  (void) current_or_previous_owner;
  (void) queue_context;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );

  _Chain_Extract_unprotected( &scheduler_node->Wait.Priority.Node.Node.Chain );
}

static void _Thread_queue_FIFO_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    queue_context,
    _Thread_queue_FIFO_do_initialize,
    _Thread_queue_FIFO_do_enqueue
  );
}

static void _Thread_queue_FIFO_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_extract(
    queue,
    queue->heads,
    NULL,
    queue_context,
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
  scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( first );

  return _Scheduler_Node_get_owner( scheduler_node );
}

static Thread_Control *_Thread_queue_FIFO_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *first;

  first = _Thread_queue_FIFO_first( heads );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    NULL,
    queue_context,
    first,
    _Thread_queue_FIFO_do_extract
  );

  return first;
}

static Thread_queue_Priority_queue *_Thread_queue_Priority_queue(
  Thread_queue_Heads   *heads,
  const Scheduler_Node *scheduler_node
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Control *scheduler;

  scheduler = _Priority_Get_scheduler( &scheduler_node->Wait.Priority );
  return &heads->Priority[ _Scheduler_Get_index( scheduler ) ];
#else
  (void) scheduler_node;
  return &heads->Heads.Priority;
#endif
}

static void _Thread_queue_Priority_priority_actions(
  Thread_queue_Queue *queue,
  Priority_Actions   *priority_actions
)
{
  Thread_queue_Heads   *heads;
  Priority_Aggregation *priority_aggregation;

  heads = queue->heads;
  _Assert( heads != NULL );

  _Assert( !_Priority_Actions_is_empty( priority_actions ) );
  priority_aggregation = _Priority_Actions_move( priority_actions );

  do {
    Scheduler_Node              *scheduler_node;
    Thread_queue_Priority_queue *priority_queue;

    scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    _Assert( priority_aggregation->Action.type == PRIORITY_ACTION_CHANGE );
    _Priority_Plain_changed(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );

    priority_aggregation = _Priority_Get_next_action( priority_aggregation );
  } while ( _Priority_Actions_is_valid( priority_aggregation ) );
}

static void _Thread_queue_Priority_do_initialize(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

#if defined(RTEMS_SMP)
  _Chain_Initialize_one( &heads->Heads.Fifo, &priority_queue->Node );
#endif

  _Priority_Initialize_one(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );
}

static void _Thread_queue_Priority_do_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

#if defined(RTEMS_SMP)
  if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
    _Priority_Initialize_one(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );
    return;
  }
#endif

  _Priority_Plain_insert(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    _Priority_Get_priority( &scheduler_node->Wait.Priority )
  );
}

static void _Thread_queue_Priority_do_extract(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *current_or_previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  (void) current_or_previous_owner;
  (void) queue_context;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Plain_extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );

#if defined(RTEMS_SMP)
  _Chain_Extract_unprotected( &priority_queue->Node );

  if ( !_Priority_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }
#endif
}

static void _Thread_queue_Priority_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    queue_context,
    _Thread_queue_Priority_do_initialize,
    _Thread_queue_Priority_do_enqueue
  );
}

static void _Thread_queue_Priority_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_extract(
    queue,
    queue->heads,
    NULL,
    queue_context,
    the_thread,
    _Thread_queue_Priority_do_extract
  );
}

static Thread_Control *_Thread_queue_Priority_first(
  Thread_queue_Heads *heads
)
{
  Thread_queue_Priority_queue *priority_queue;
  Priority_Node               *first;
  Scheduler_Node              *scheduler_node;

#if defined(RTEMS_SMP)
  _Assert( !_Chain_Is_empty( &heads->Heads.Fifo ) );
  priority_queue = (Thread_queue_Priority_queue *)
    _Chain_First( &heads->Heads.Fifo );
#else
  priority_queue = &heads->Heads.Priority;
#endif

  _Assert( !_Priority_Is_empty( &priority_queue->Queue ) );
  first = _Priority_Get_minimum_node( &priority_queue->Queue );
  scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( first );

  return _Scheduler_Node_get_owner( scheduler_node );
}

static Thread_Control *_Thread_queue_Priority_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *first;

  first = _Thread_queue_Priority_first( heads );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    NULL,
    queue_context,
    first,
    _Thread_queue_Priority_do_extract
  );

  return first;
}

static void _Thread_queue_Priority_inherit_do_actions_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node;

  priority_queue = THREAD_QUEUE_PRIORITY_QUEUE_OF_PRIORITY_AGGREGATION(
    priority_aggregation
  );
  scheduler_node = priority_queue->scheduler_node;

  _Priority_Set_action(
    &scheduler_node->Wait.Priority,
    &priority_aggregation->Node,
    PRIORITY_ACTION_CHANGE
  );
  _Priority_Actions_add( priority_actions, &scheduler_node->Wait.Priority );
}

static void _Thread_queue_Priority_inherit_priority_actions(
  Thread_queue_Queue *queue,
  Priority_Actions   *priority_actions
)
{
  Thread_queue_Heads   *heads;
  Priority_Aggregation *priority_aggregation;

  heads = queue->heads;
  _Assert( heads != NULL );

  _Assert( !_Priority_Actions_is_empty( priority_actions ) );
  priority_aggregation = _Priority_Actions_move( priority_actions );

  do {
    Priority_Aggregation        *next_aggregation;
    Scheduler_Node              *scheduler_node;
    Thread_queue_Priority_queue *priority_queue;

    next_aggregation = _Priority_Get_next_action( priority_aggregation );

    scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    _Assert( priority_aggregation->Action.type == PRIORITY_ACTION_CHANGE );
    _Priority_Changed(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node,
      false,
      priority_actions,
      _Thread_queue_Priority_inherit_do_actions_change,
      NULL
    );

    priority_aggregation = next_aggregation;
  } while ( _Priority_Actions_is_valid( priority_aggregation ) );
}

static void _Thread_queue_Boost_priority(
  Thread_queue_Heads   *heads,
  Thread_Control       *the_thread,
  Thread_Control       *owner,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Control *scheduler;
  const Scheduler_Control *scheduler_of_owner;
  Scheduler_Node          *scheduler_node_of_owner;
  Priority_Control         boost_priority;

  if ( _Priority_Node_is_active( &heads->Boost_priority ) ) {
    return;
  }

  scheduler = _Scheduler_Get_own( the_thread );
  scheduler_of_owner = _Scheduler_Get_own( owner );

  if ( scheduler == scheduler_of_owner ) {
    return;
  }

  scheduler_node_of_owner = _Thread_Scheduler_get_own_node( owner );

  boost_priority = _Scheduler_Map_priority(
    scheduler_of_owner,
    PRIORITY_PSEUDO_ISR
  );

  _Priority_Node_initialize( &heads->Boost_priority, boost_priority );
  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &heads->Boost_priority,
    PRIORITY_ACTION_ADD
  );
  _Thread_Priority_perform_actions( owner, queue_context );
#else
  (void) heads;
  (void) the_thread;
  (void) owner;
  (void) queue_context;
#endif
}

static void _Thread_queue_Priority_inherit_do_initialize(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;
  Thread_Control              *owner;
  Scheduler_Node              *scheduler_node_of_owner;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

#if defined(RTEMS_SMP)
  _Chain_Initialize_one( &heads->Heads.Fifo, &priority_queue->Node );
#endif

  _Priority_Initialize_one(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );

  owner = queue->owner;
  scheduler_node_of_owner = _Thread_Scheduler_get_own_node( owner );
  priority_queue->scheduler_node = scheduler_node_of_owner;

  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &priority_queue->Queue.Node,
    PRIORITY_ACTION_ADD
  );
  _Thread_Priority_perform_actions( owner, queue_context );
  _Thread_queue_Boost_priority( heads, the_thread, owner, queue_context );
}

static void _Thread_queue_Priority_inherit_do_enqueue_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  Thread_queue_Queue   *queue;
  Thread_Control       *owner;
  Scheduler_Node       *scheduler_node_of_owner;
  Thread_queue_Context *queue_context;

  queue = arg;
  owner = queue->owner;
  scheduler_node_of_owner = _Thread_Scheduler_get_own_node( owner );
  queue_context = THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions );

  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    PRIORITY_ACTION_CHANGE
  );
  _Thread_Priority_perform_actions( owner, queue_context );
}

static void _Thread_queue_Priority_inherit_do_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

#if defined(RTEMS_SMP)
  if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
    Thread_Control *owner;
    Scheduler_Node *scheduler_node_of_owner;

    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
    _Priority_Initialize_one(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );

    owner = queue->owner;
    scheduler_node_of_owner = _Thread_Scheduler_get_own_node( owner );
    priority_queue->scheduler_node = scheduler_node_of_owner;

    _Priority_Actions_initialize_one(
      &queue_context->Priority.Actions,
      &scheduler_node_of_owner->Wait.Priority,
      &priority_queue->Queue.Node,
      PRIORITY_ACTION_ADD
    );
    _Thread_Priority_perform_actions( owner, queue_context );
    _Thread_queue_Boost_priority( heads, the_thread, owner, queue_context );
    return;
  }
#endif

  _Priority_Non_empty_insert(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_enqueue_change,
    queue
  );
  _Thread_queue_Boost_priority(
    heads,
    the_thread,
    queue->owner,
    queue_context
  );
}

static void _Thread_queue_Priority_inherit_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_enqueue(
    queue,
    the_thread,
    queue_context,
    _Thread_queue_Priority_inherit_do_initialize,
    _Thread_queue_Priority_inherit_do_enqueue
  );
}

static void _Thread_queue_Priority_inherit_do_extract_action(
  Priority_Actions     *priority_actions,
  Thread_Control       *owner,
  Priority_Node        *priority_action_node,
  Priority_Action_type  priority_action_type
)
{
  Thread_queue_Context *queue_context;
  Scheduler_Node       *scheduler_node_of_owner;

  queue_context = THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions );
  scheduler_node_of_owner = _Thread_Scheduler_get_own_node( owner );

  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    priority_action_node,
    priority_action_type
  );
  _Thread_Priority_perform_actions( owner, queue_context );
}

static void _Thread_queue_Priority_inherit_do_extract_remove(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Priority_inherit_do_extract_action(
    priority_actions,
    arg,
    &priority_aggregation->Node,
    PRIORITY_ACTION_REMOVE
  );
}

static void _Thread_queue_Priority_inherit_do_extract_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Priority_inherit_do_extract_action(
    priority_actions,
    arg,
    &priority_aggregation->Node,
    PRIORITY_ACTION_CHANGE
  );
}

static void _Thread_queue_Priority_inherit_do_extract(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Thread_queue_Path_acquire_critical( queue, the_thread, queue_context );

  _Priority_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_extract_remove,
    _Thread_queue_Priority_inherit_do_extract_change,
    owner
  );

#if defined(RTEMS_SMP)
  _Chain_Extract_unprotected( &priority_queue->Node );

  if ( !_Priority_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }

  if (
    _Priority_Node_is_active( &heads->Boost_priority )
      && ( _Chain_Is_empty( &heads->Heads.Fifo )
        || _Chain_Has_only_one_node( &heads->Heads.Fifo ) )
  ) {
    _Thread_queue_Priority_inherit_do_extract_action(
      &queue_context->Priority.Actions,
      owner,
      &heads->Boost_priority,
      PRIORITY_ACTION_REMOVE
    );
    _Priority_Node_set_inactive( &heads->Boost_priority );
  }
#endif

  _Thread_queue_Path_release_critical( queue_context );
}

static void _Thread_queue_Priority_inherit_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_extract(
    queue,
    queue->heads,
    queue->owner,
    queue_context,
    the_thread,
    _Thread_queue_Priority_inherit_do_extract
  );
}

static void _Thread_queue_Priority_inherit_do_surrender_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Context_add_priority_update(
    THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions ),
    arg
  );
  _Scheduler_Node_set_priority(
    SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation ),
    _Priority_Get_priority( priority_aggregation ),
    prepend_it
  );
}

static void _Thread_queue_Priority_add(
  Thread_Control       *the_thread,
  Priority_Aggregation *priority_aggregation,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  _Priority_Non_empty_insert(
    priority_aggregation,
    priority_node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_surrender_change,
    the_thread
  );
}

static void _Thread_queue_Priority_remove(
  Thread_Control       *the_thread,
  Scheduler_Node       *scheduler_node,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  _Priority_Extract_non_empty(
    &scheduler_node->Wait.Priority,
    priority_node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_surrender_change,
    the_thread
  );
}

static void _Thread_queue_Priority_inherit_do_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;
  ISR_lock_Context             lock_context;

  scheduler_node = _Thread_Scheduler_get_own_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Thread_Wait_acquire_default_critical( previous_owner, &lock_context );

#if defined(RTEMS_SMP)
  if ( _Priority_Node_is_active( &heads->Boost_priority ) ) {
    _Thread_queue_Priority_remove(
      previous_owner,
      _Thread_Scheduler_get_own_node( previous_owner ),
      &heads->Boost_priority,
      queue_context
    );
    _Priority_Node_set_inactive( &heads->Boost_priority );
  }
#endif

  _Thread_queue_Priority_remove(
    previous_owner,
    priority_queue->scheduler_node,
    &priority_queue->Queue.Node,
    queue_context
  );

  _Thread_Wait_release_default_critical( previous_owner, &lock_context );

  _Priority_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    NULL,
    _Priority_Remove_nothing,
    _Priority_Change_nothing,
    previous_owner
  );

  if ( !_Priority_Is_empty( &priority_queue->Queue ) ) {
    priority_queue->scheduler_node = scheduler_node;
    _Thread_queue_Priority_add(
      the_thread,
      &scheduler_node->Wait.Priority,
      &priority_queue->Queue.Node,
      queue_context
    );
  }

#if defined(RTEMS_SMP)
  _Chain_Extract_unprotected( &priority_queue->Node );

  if ( !_Priority_Is_empty( &priority_queue->Queue ) ) {
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
  }

  if (
    !_Chain_Is_empty( &heads->Heads.Fifo)
      && !_Chain_Has_only_one_node( &heads->Heads.Fifo)
  ) {
    Priority_Control boost_priority;

    boost_priority = _Scheduler_Map_priority(
      _Scheduler_Get_own( the_thread ),
      PRIORITY_PSEUDO_ISR
    );
    _Priority_Node_initialize( &heads->Boost_priority, boost_priority );
    _Thread_queue_Priority_add(
      the_thread,
      &scheduler_node->Wait.Priority,
      &heads->Boost_priority,
      queue_context
    );
  }
#endif
}

static Thread_Control *_Thread_queue_Priority_inherit_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *first;

  first = _Thread_queue_Priority_first( heads );
  _Thread_queue_Queue_extract(
    queue,
    heads,
    previous_owner,
    queue_context,
    first,
    _Thread_queue_Priority_inherit_do_surrender
  );

  return first;
}

const Thread_queue_Operations _Thread_queue_Operations_default = {
  .priority_actions = _Thread_queue_Do_nothing_priority_actions,
  .extract = _Thread_queue_Do_nothing_extract
  /*
   * The default operations are only used in _Thread_Change_priority() and
   * _Thread_Timeout() and don't have a thread queue associated with them, so
   * the enqueue and first operations are superfluous.
   */
};

const Thread_queue_Operations _Thread_queue_Operations_FIFO = {
  .priority_actions = _Thread_queue_Do_nothing_priority_actions,
  .enqueue = _Thread_queue_FIFO_enqueue,
  .extract = _Thread_queue_FIFO_extract,
  .surrender = _Thread_queue_FIFO_surrender,
  .first = _Thread_queue_FIFO_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority = {
  .priority_actions = _Thread_queue_Priority_priority_actions,
  .enqueue = _Thread_queue_Priority_enqueue,
  .extract = _Thread_queue_Priority_extract,
  .surrender = _Thread_queue_Priority_surrender,
  .first = _Thread_queue_Priority_first
};

const Thread_queue_Operations _Thread_queue_Operations_priority_inherit = {
  .priority_actions = _Thread_queue_Priority_inherit_priority_actions,
  .enqueue = _Thread_queue_Priority_inherit_enqueue,
  .extract = _Thread_queue_Priority_inherit_extract,
  .surrender = _Thread_queue_Priority_inherit_surrender,
  .first = _Thread_queue_Priority_first
};
