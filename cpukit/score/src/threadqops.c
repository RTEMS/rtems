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

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );

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

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );

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

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );

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

static size_t _Thread_queue_Scheduler_index(
  const Scheduler_Node *scheduler_node
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Control *scheduler;

  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );
  return _Scheduler_Get_index( scheduler );
#else
  (void) scheduler_node;
  return 0;
#endif
}

static Thread_queue_Priority_queue *_Thread_queue_Priority_queue_by_index(
  Thread_queue_Heads *heads,
  size_t              scheduler_index
)
{
#if defined(RTEMS_SMP)
  return &heads->Priority[ scheduler_index ];
#else
  (void) scheduler_index;
  return &heads->Heads.Priority;
#endif
}

static Thread_queue_Priority_queue *_Thread_queue_Priority_queue(
  Thread_queue_Heads   *heads,
  const Scheduler_Node *scheduler_node
)
{
  return _Thread_queue_Priority_queue_by_index(
    heads,
    _Thread_queue_Scheduler_index( scheduler_node )
  );
}

static Chain_Node *_Thread_queue_Priority_queue_rotation(
  Thread_queue_Heads *heads
)
{
  Chain_Node *fifo_node;

#if defined(RTEMS_SMP)
  /* Ensure FIFO order with respect to the priority queues */
  fifo_node = _Chain_First( &heads->Heads.Fifo );
  _Chain_Extract_unprotected( fifo_node );
  _Chain_Append_unprotected( &heads->Heads.Fifo, fifo_node );
#else
  (void) heads;
  fifo_node = NULL;
#endif

  return fifo_node;
}

#if defined(RTEMS_SMP)
static void _Thread_queue_Priority_queue_extract(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  Thread_queue_Priority_queue *priority_queue;

  (void) priority_actions;
  (void) arg;

  priority_queue = THREAD_QUEUE_PRIORITY_QUEUE_OF_PRIORITY_AGGREGATION(
    priority_aggregation
  );

  _Chain_Extract_unprotected( &priority_queue->Node );
}
#endif

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
    Priority_Action_type         priority_action_type;

    scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );
    priority_action_type = priority_aggregation->Action.type;

    switch ( priority_action_type ) {
#if defined(RTEMS_SMP)
      case PRIORITY_ACTION_ADD:
        if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
          _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
        }

        _Priority_Plain_insert(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node,
          _Priority_Get_priority( &scheduler_node->Wait.Priority )
        );
        break;
      case PRIORITY_ACTION_REMOVE:
        _Priority_Plain_extract(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node
        );

        if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
          _Chain_Extract_unprotected( &priority_queue->Node );
        }
        break;
#endif
      default:
        _Assert( priority_action_type == PRIORITY_ACTION_CHANGE );
        _Priority_Plain_changed(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node
        );
        break;
    }

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
#if defined(RTEMS_SMP)
  Chain_Node                  *wait_node;
  const Chain_Node            *wait_tail;
#endif

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Initialize_one(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );

#if defined(RTEMS_SMP)
  _Chain_Initialize_one( &heads->Heads.Fifo, &priority_queue->Node );

  wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  while ( wait_node != wait_tail ) {
    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    _Priority_Initialize_one(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );
    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  }
#endif
}

static void _Thread_queue_Priority_do_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
#if defined(RTEMS_SMP)
  Chain_Node       *wait_node;
  const Chain_Node *wait_tail;

  wait_node = _Chain_First( &the_thread->Scheduler.Wait_nodes );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  do {
    Scheduler_Node              *scheduler_node;
    Thread_queue_Priority_queue *priority_queue;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
      _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
      _Priority_Initialize_one(
        &priority_queue->Queue,
        &scheduler_node->Wait.Priority.Node
      );
    } else {
      _Priority_Plain_insert(
        &priority_queue->Queue,
        &scheduler_node->Wait.Priority.Node,
        _Priority_Get_priority( &scheduler_node->Wait.Priority )
      );
    }

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  } while ( wait_node != wait_tail );
#else
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Plain_insert(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    _Priority_Get_priority( &scheduler_node->Wait.Priority )
  );
#endif
}

static void _Thread_queue_Priority_do_extract(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *current_or_previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
#if defined(RTEMS_SMP)
  Chain_Node       *wait_node;
  const Chain_Node *wait_tail;

  wait_node = _Chain_First( &the_thread->Scheduler.Wait_nodes );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  do {
    Scheduler_Node              *scheduler_node;
    Thread_queue_Priority_queue *priority_queue;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    _Priority_Plain_extract(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );

    if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
      _Chain_Extract_unprotected( &priority_queue->Node );
    }

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  } while ( wait_node != wait_tail );
#else
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Plain_extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );
#endif

  (void) current_or_previous_owner;
  (void) queue_context;
}

static void _Thread_queue_Priority_do_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *current_or_previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
  _Thread_queue_Priority_queue_rotation( heads );
  _Thread_queue_Priority_do_extract(
    queue,
    heads,
    current_or_previous_owner,
    queue_context,
    the_thread
  );
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
    _Thread_queue_Priority_do_surrender
  );

  return first;
}

static void _Thread_queue_Priority_inherit_do_priority_actions_action(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  Scheduler_Node       *scheduler_node_of_owner,
  Priority_Action_type  priority_action_type
)
{
  _Priority_Set_action(
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    priority_action_type
  );
  _Priority_Actions_add(
    priority_actions,
    &scheduler_node_of_owner->Wait.Priority
  );
}

#if defined(RTEMS_SMP)
static void _Thread_queue_Priority_inherit_do_priority_actions_add(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Priority_inherit_do_priority_actions_action(
    priority_aggregation,
    priority_actions,
    arg,
    PRIORITY_ACTION_ADD
  );
}

static void _Thread_queue_Priority_inherit_do_priority_actions_remove(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Priority_queue_extract(
    priority_aggregation,
    priority_actions,
    arg
  );
  _Thread_queue_Priority_inherit_do_priority_actions_action(
    priority_aggregation,
    priority_actions,
    arg,
    PRIORITY_ACTION_REMOVE
  );
}
#endif

static void _Thread_queue_Priority_inherit_do_priority_actions_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_queue_Priority_inherit_do_priority_actions_action(
    priority_aggregation,
    priority_actions,
    arg,
    PRIORITY_ACTION_CHANGE
  );
}

static void _Thread_queue_Priority_inherit_priority_actions(
  Thread_queue_Queue *queue,
  Priority_Actions   *priority_actions
)
{
  Thread_queue_Heads   *heads;
  Thread_Control       *owner;
  Priority_Aggregation *priority_aggregation;

  heads = queue->heads;
  _Assert( heads != NULL );

  owner = queue->owner;
  _Assert( owner != NULL );

  _Assert( !_Priority_Actions_is_empty( priority_actions ) );
  priority_aggregation = _Priority_Actions_move( priority_actions );

  do {
    Priority_Aggregation        *next_aggregation;
    Scheduler_Node              *scheduler_node;
    size_t                       scheduler_index;
    Thread_queue_Priority_queue *priority_queue;
    Scheduler_Node              *scheduler_node_of_owner;
    Priority_Action_type         priority_action_type;

    next_aggregation = _Priority_Get_next_action( priority_aggregation );

    scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
    scheduler_index = _Thread_queue_Scheduler_index( scheduler_node );
    priority_queue = _Thread_queue_Priority_queue_by_index(
      heads,
      scheduler_index
    );
    scheduler_node_of_owner = _Thread_Scheduler_get_node_by_index(
      owner,
      scheduler_index
    );
    priority_action_type = priority_aggregation->Action.type;

    switch ( priority_action_type ) {
#if defined(RTEMS_SMP)
      case PRIORITY_ACTION_ADD:
        if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
          _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
          priority_queue->scheduler_node = scheduler_node_of_owner;
        }

        _Priority_Insert(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node,
          priority_actions,
          _Thread_queue_Priority_inherit_do_priority_actions_add,
          _Thread_queue_Priority_inherit_do_priority_actions_change,
          scheduler_node_of_owner
        );
        break;
      case PRIORITY_ACTION_REMOVE:
        _Priority_Extract(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node,
          priority_actions,
          _Thread_queue_Priority_inherit_do_priority_actions_remove,
          _Thread_queue_Priority_inherit_do_priority_actions_change,
          scheduler_node_of_owner
        );

        break;
#endif
      default:
        _Assert( priority_action_type == PRIORITY_ACTION_CHANGE );
        _Priority_Changed(
          &priority_queue->Queue,
          &scheduler_node->Wait.Priority.Node,
          false,
          priority_actions,
          _Thread_queue_Priority_inherit_do_priority_actions_change,
          scheduler_node_of_owner
        );
        break;
    }

    priority_aggregation = next_aggregation;
  } while ( _Priority_Actions_is_valid( priority_aggregation ) );
}

static void _Thread_queue_Priority_inherit_do_initialize(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
  Scheduler_Node              *scheduler_node;
  size_t                       scheduler_index;
  Thread_queue_Priority_queue *priority_queue;
  Thread_Control              *owner;
  Scheduler_Node              *scheduler_node_of_owner;
#if defined(RTEMS_SMP)
  Chain_Node                  *wait_node;
  const Chain_Node            *wait_tail;
#endif

  owner = queue->owner;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  scheduler_index = _Thread_queue_Scheduler_index( scheduler_node );
  priority_queue = _Thread_queue_Priority_queue_by_index(
    heads,
    scheduler_index
  );
  scheduler_node_of_owner = _Thread_Scheduler_get_node_by_index(
    owner,
    scheduler_index
  );

  priority_queue->scheduler_node = scheduler_node_of_owner;
  _Priority_Initialize_one(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node
  );
  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &priority_queue->Queue.Node,
    PRIORITY_ACTION_ADD
  );

#if defined(RTEMS_SMP)
  _Chain_Initialize_one( &heads->Heads.Fifo, &priority_queue->Node );

  wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  while ( wait_node != wait_tail ) {
    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    scheduler_index = _Thread_queue_Scheduler_index( scheduler_node );
    priority_queue = _Thread_queue_Priority_queue_by_index(
      heads,
      scheduler_index
    );
    scheduler_node_of_owner = _Thread_Scheduler_get_node_by_index(
      owner,
      scheduler_index
    );

    _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
    priority_queue->scheduler_node = scheduler_node_of_owner;
    _Priority_Initialize_one(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node
    );
    _Priority_Set_action(
      &scheduler_node_of_owner->Wait.Priority,
      &priority_queue->Queue.Node,
      PRIORITY_ACTION_ADD
    );
    _Priority_Actions_add(
      &queue_context->Priority.Actions,
      &scheduler_node_of_owner->Wait.Priority
    );

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  }
#endif

  _Thread_Priority_perform_actions( owner, queue_context );
}

static void _Thread_queue_Priority_inherit_do_enqueue_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
#if defined(RTEMS_SMP)
  Scheduler_Node *scheduler_node_of_owner;

  scheduler_node_of_owner = arg;

  _Priority_Set_action(
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    PRIORITY_ACTION_CHANGE
  );
  _Priority_Actions_add(
    priority_actions,
    &scheduler_node_of_owner->Wait.Priority
  );
#else
  Thread_queue_Queue   *queue;
  Thread_Control       *owner;
  Scheduler_Node       *scheduler_node_of_owner;
  Thread_queue_Context *queue_context;

  queue = arg;
  owner = queue->owner;
  scheduler_node_of_owner = _Thread_Scheduler_get_home_node( owner );
  queue_context = THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions );

  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    PRIORITY_ACTION_CHANGE
  );
  _Thread_Priority_perform_actions( owner, queue_context );
#endif
}

static void _Thread_queue_Priority_inherit_do_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context,
  Thread_queue_Heads   *heads
)
{
#if defined(RTEMS_SMP)
  Thread_Control   *owner;
  Chain_Node       *wait_node;
  const Chain_Node *wait_tail;

  owner = queue->owner;
  wait_node = _Chain_First( &the_thread->Scheduler.Wait_nodes );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  _Priority_Actions_initialize_empty( &queue_context->Priority.Actions );

  do {
    Scheduler_Node              *scheduler_node;
    size_t                       scheduler_index;
    Thread_queue_Priority_queue *priority_queue;
    Scheduler_Node              *scheduler_node_of_owner;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    scheduler_index = _Thread_queue_Scheduler_index( scheduler_node );
    priority_queue = _Thread_queue_Priority_queue_by_index(
      heads,
      scheduler_index
    );
    scheduler_node_of_owner = _Thread_Scheduler_get_node_by_index(
      owner,
      scheduler_index
    );

    if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
      _Chain_Append_unprotected( &heads->Heads.Fifo, &priority_queue->Node );
      priority_queue->scheduler_node = scheduler_node_of_owner;
      _Priority_Initialize_one(
        &priority_queue->Queue,
        &scheduler_node->Wait.Priority.Node
      );
      _Priority_Set_action(
        &scheduler_node_of_owner->Wait.Priority,
        &priority_queue->Queue.Node,
        PRIORITY_ACTION_ADD
      );
      _Priority_Actions_add(
        &queue_context->Priority.Actions,
        &scheduler_node_of_owner->Wait.Priority
      );
    } else {
      _Priority_Non_empty_insert(
        &priority_queue->Queue,
        &scheduler_node->Wait.Priority.Node,
        &queue_context->Priority.Actions,
        _Thread_queue_Priority_inherit_do_enqueue_change,
        scheduler_node_of_owner
      );
    }

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  } while ( wait_node != wait_tail );

  if ( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
    _Thread_Priority_perform_actions( owner, queue_context );
  }
#else
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Non_empty_insert(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_enqueue_change,
    queue
  );
#endif
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
  void                 *arg,
  Priority_Aggregation *priority_aggregation,
  Priority_Action_type  priority_action_type
)
{
#if defined(RTEMS_SMP)
  Scheduler_Node *scheduler_node_of_owner;

  scheduler_node_of_owner = arg;

  _Priority_Set_action(
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    priority_action_type
  );
  _Priority_Actions_add(
    priority_actions,
    &scheduler_node_of_owner->Wait.Priority
  );
#else
  Thread_queue_Context *queue_context;
  Thread_Control       *owner;
  Scheduler_Node       *scheduler_node_of_owner;

  queue_context = THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions );
  owner = arg;
  scheduler_node_of_owner = _Thread_Scheduler_get_home_node( owner );

  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node_of_owner->Wait.Priority,
    &priority_aggregation->Node,
    priority_action_type
  );
  _Thread_Priority_perform_actions( arg, queue_context );
#endif
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
    priority_aggregation,
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
    priority_aggregation,
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
#if defined(RTEMS_SMP)
  Chain_Node                  *wait_node;
  const Chain_Node            *wait_tail;
#endif
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;

#if defined(RTEMS_SMP)
  wait_node = _Chain_First( &the_thread->Scheduler.Wait_nodes );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  _Priority_Actions_initialize_empty( &queue_context->Priority.Actions );

  do {
    size_t          scheduler_index;
    Scheduler_Node *scheduler_node_of_owner;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    scheduler_index = _Thread_queue_Scheduler_index( scheduler_node );
    priority_queue = _Thread_queue_Priority_queue_by_index(
      heads,
      scheduler_index
    );
    scheduler_node_of_owner = _Thread_Scheduler_get_node_by_index(
      owner,
      scheduler_index
    );

    _Priority_Extract(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node,
      &queue_context->Priority.Actions,
      _Thread_queue_Priority_inherit_do_extract_remove,
      _Thread_queue_Priority_inherit_do_extract_change,
      scheduler_node_of_owner
    );

    if ( _Priority_Is_empty( &priority_queue->Queue ) ) {
      _Chain_Extract_unprotected( &priority_queue->Node );
    }

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  } while ( wait_node != wait_tail );

  if ( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
    _Thread_Priority_perform_actions( owner, queue_context );
  }
#else
  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

  _Priority_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_extract_remove,
    _Thread_queue_Priority_inherit_do_extract_change,
    owner
  );
#endif
}

static void _Thread_queue_Priority_inherit_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  /*
   * We must lock the thread wait path for the complete extract operation
   * including the thread queue head management.  Consider the following
   * scenario on three processors.  Thread T0 owns thread queue A, thread T1
   * owns thread queue B and thread T2 owns thread queue C.  Thread T0 waits
   * for B and thread T1 waits for C.
   *
   * A <-------------------------\
   *  \                          |
   *   > T0 -> B                 |
   *            \                |
   *             > T1 -> C       |
   *                      \      |
   *                       > T2 -/
   *
   * Now three things happen at the same time
   *  - thread T0 times out,
   *  - thread T1 times out,
   *  - thread T2 tries to enqueue on a thread queue A.
   *
   * Thread T1 acquires thread queue lock C and waits for thread queue lock A.
   * Thread T2 acquires thread queue lock A and waits for thread queue lock B.
   * Thread T0 acquires thread queue lock B and detects a potential deadlock.
   * Thread T0 carries out the thread queue extraction due to the timeout and
   * uses the thread wait path segments acquired by thread T1 and T2.  This
   * resolves the deadlock.  Thread T1 and T2 can the complete their
   * operations.
   */
  _Thread_queue_Path_acquire_critical( queue, the_thread, queue_context );
#endif

  _Thread_queue_Queue_extract(
    queue,
    queue->heads,
    queue->owner,
    queue_context,
    the_thread,
    _Thread_queue_Priority_inherit_do_extract
  );

#if defined(RTEMS_SMP)
  _Thread_queue_Path_release_critical( queue_context );
#endif
}

#if defined(RTEMS_SMP)
static void _Thread_queue_Priority_inherit_do_surrender_add(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  Scheduler_Node *scheduler_node;
  Thread_Control *the_thread;

  scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
  the_thread = arg;

  _Thread_Scheduler_add_wait_node( the_thread, scheduler_node );
  _Scheduler_Node_set_priority(
    scheduler_node,
    _Priority_Get_priority( priority_aggregation ),
    false
  );
}

static void _Thread_queue_Priority_inherit_do_surrender_remove(
  Priority_Aggregation *priority_aggregation,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  Scheduler_Node *scheduler_node;
  Thread_Control *the_thread;

  scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation );
  the_thread = arg;

  _Thread_Scheduler_remove_wait_node( the_thread, scheduler_node );
  _Priority_Actions_add( priority_actions, priority_aggregation );
}
#endif

static void _Thread_queue_Priority_inherit_do_surrender_change(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
#if defined(RTEMS_SMP)
  _Priority_Actions_add( priority_actions, priority_aggregation );
#else
  _Thread_queue_Context_add_priority_update(
    THREAD_QUEUE_CONTEXT_OF_PRIORITY_ACTIONS( priority_actions ),
    arg
  );
#endif
  _Scheduler_Node_set_priority(
    SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation ),
    _Priority_Get_priority( priority_aggregation ),
    prepend_it
  );
}

#if defined(RTEMS_SMP)
static void _Thread_queue_Priority_inherit_do_surrender_change_2(
  Priority_Aggregation *priority_aggregation,
  bool                  prepend_it,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Scheduler_Node_set_priority(
    SCHEDULER_NODE_OF_WAIT_PRIORITY( priority_aggregation ),
    _Priority_Get_priority( priority_aggregation ),
    prepend_it
  );
}
#endif

static void _Thread_queue_Priority_inherit_do_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context,
  Thread_Control       *the_thread
)
{
#if defined(RTEMS_SMP)
  Chain_Node                  *fifo_node;
  const Chain_Node            *fifo_head;
  const Chain_Node            *fifo_tail;
  Chain_Node                  *wait_node;
  const Chain_Node            *wait_tail;
  ISR_lock_Context             lock_context;
#endif
  Scheduler_Node              *scheduler_node;
  Thread_queue_Priority_queue *priority_queue;
  Scheduler_Node              *scheduler_node_of_owner;

#if defined(RTEMS_SMP)
  /*
   * Remove the priority node of each priority queue from the previous owner.
   * If a priority changes due to this, then register it for a priority update.
   */

  fifo_node = _Thread_queue_Priority_queue_rotation( heads );
  fifo_head = _Chain_Immutable_head( &heads->Heads.Fifo );

  _Priority_Actions_initialize_empty( &queue_context->Priority.Actions );

  _Thread_Wait_acquire_default_critical( previous_owner, &lock_context );

  do {
    priority_queue = (Thread_queue_Priority_queue *) fifo_node;
    scheduler_node_of_owner = priority_queue->scheduler_node;

    _Assert( scheduler_node_of_owner->owner == previous_owner );

    _Priority_Extract(
      &scheduler_node_of_owner->Wait.Priority,
      &priority_queue->Queue.Node,
      &queue_context->Priority.Actions,
      _Thread_queue_Priority_inherit_do_surrender_remove,
      _Thread_queue_Priority_inherit_do_surrender_change,
      previous_owner
    );

    fifo_node = _Chain_Previous( fifo_node );
  } while ( fifo_node != fifo_head );

  if ( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
    /*
     * The previous owner performs this surrender operation.  So, it is
     * definitely not enqueued on a thread queue.  It is sufficient to notify
     * the scheduler about a priority update.  There is no need for a
     * _Thread_Priority_perform_actions().
     */
    _Thread_queue_Context_add_priority_update( queue_context, previous_owner );
  }

  _Thread_Wait_release_default_critical( previous_owner, &lock_context );

  /*
   * Remove the wait node of the new owner from the corresponding priority
   * queue.
   */

  wait_node = _Chain_First( &the_thread->Scheduler.Wait_nodes );
  wait_tail = _Chain_Immutable_tail( &the_thread->Scheduler.Wait_nodes );

  do {
    scheduler_node = SCHEDULER_NODE_OF_THREAD_WAIT_NODE( wait_node );
    priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );

    _Priority_Extract(
      &priority_queue->Queue,
      &scheduler_node->Wait.Priority.Node,
      NULL,
      _Thread_queue_Priority_queue_extract,
      _Priority_Change_nothing,
      NULL
    );

    wait_node = _Chain_Next( &scheduler_node->Thread.Wait_node );
  } while ( wait_node != wait_tail );

  /* Add the priority node of the remaining priority queues to the new owner */

  fifo_node = _Chain_First( &heads->Heads.Fifo );
  fifo_tail = _Chain_Immutable_tail( &heads->Heads.Fifo );

  while ( fifo_node != fifo_tail ) {
    const Scheduler_Control *scheduler;

    priority_queue = (Thread_queue_Priority_queue *) fifo_node;
    scheduler = _Priority_Get_scheduler( &priority_queue->Queue );
    scheduler_node = _Thread_Scheduler_get_node_by_index(
      the_thread,
      _Scheduler_Get_index( scheduler )
    );

    priority_queue->scheduler_node = scheduler_node;
    _Priority_Insert(
      &scheduler_node->Wait.Priority,
      &priority_queue->Queue.Node,
      &queue_context->Priority.Actions,
      _Thread_queue_Priority_inherit_do_surrender_add,
      _Thread_queue_Priority_inherit_do_surrender_change_2,
      the_thread
    );

    fifo_node = _Chain_Next( fifo_node );
  }
#else
  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  priority_queue = _Thread_queue_Priority_queue( heads, scheduler_node );
  scheduler_node_of_owner = priority_queue->scheduler_node;

  _Priority_Extract_non_empty(
    &scheduler_node_of_owner->Wait.Priority,
    &priority_queue->Queue.Node,
    &queue_context->Priority.Actions,
    _Thread_queue_Priority_inherit_do_surrender_change,
    previous_owner
  );
  _Priority_Extract(
    &priority_queue->Queue,
    &scheduler_node->Wait.Priority.Node,
    NULL,
    _Priority_Remove_nothing,
    _Priority_Change_nothing,
    NULL
  );

  if ( !_Priority_Is_empty( &priority_queue->Queue ) ) {
    priority_queue->scheduler_node = scheduler_node;
    _Priority_Non_empty_insert(
      &scheduler_node->Wait.Priority,
      &priority_queue->Queue.Node,
      &queue_context->Priority.Actions,
      _Thread_queue_Priority_inherit_do_surrender_change,
      the_thread
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
