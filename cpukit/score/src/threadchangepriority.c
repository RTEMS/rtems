/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Priority_add(), _Thread_Priority_and_sticky_update(),
 *   _Thread_Priority_changed(), _Thread_Priority_perform_actions(),
 *   _Thread_Priority_remove(), _Thread_Priority_replace(), and
 *   _Thread_Priority_update().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>

static void _Thread_Set_scheduler_node_priority(
  Priority_Aggregation *priority_aggregation,
  Priority_Group_order  priority_group_order
)
{
  _Scheduler_Node_set_priority(
    SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( priority_aggregation ),
    _Priority_Get_priority( priority_aggregation ),
    priority_group_order
  );
}

#if defined(RTEMS_SMP)
static void _Thread_Priority_action_add(
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
  _Thread_Set_scheduler_node_priority(
    priority_aggregation,
    PRIORITY_GROUP_LAST
  );
  _Priority_Set_action_type( priority_aggregation, PRIORITY_ACTION_ADD );
  _Priority_Actions_add( priority_actions, priority_aggregation );
}

static void _Thread_Priority_action_remove(
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
  _Thread_Set_scheduler_node_priority(
    priority_aggregation,
    PRIORITY_GROUP_FIRST
  );
  _Priority_Set_action_type( priority_aggregation, PRIORITY_ACTION_REMOVE );
  _Priority_Actions_add( priority_actions, priority_aggregation );
}
#endif

static void _Thread_Priority_action_change(
  Priority_Aggregation *priority_aggregation,
  Priority_Group_order  priority_group_order,
  Priority_Actions     *priority_actions,
  void                 *arg
)
{
  _Thread_Set_scheduler_node_priority(
    priority_aggregation,
    priority_group_order
  );
#if defined(RTEMS_SMP) || defined(RTEMS_DEBUG)
  _Priority_Set_action_type( priority_aggregation, PRIORITY_ACTION_CHANGE );
#endif
  _Priority_Actions_add( priority_actions, priority_aggregation );
}

static void _Thread_Priority_do_perform_actions(
  Thread_Control                *the_thread,
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Priority_Group_order           priority_group_order,
  Thread_queue_Context          *queue_context
)
{
  Priority_Aggregation *priority_aggregation;

  _Assert( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) );
  priority_aggregation = _Priority_Actions_move( &queue_context->Priority.Actions );

  do {
#if defined(RTEMS_SMP)
    Priority_Aggregation *next_aggregation;
#endif
    Priority_Node        *priority_action_node;
    Priority_Action_type  priority_action_type;

#if defined(RTEMS_SMP)
    next_aggregation = _Priority_Get_next_action( priority_aggregation );
#endif

    priority_action_node = priority_aggregation->Action.node;
    priority_action_type = priority_aggregation->Action.type;

    switch ( priority_action_type ) {
      case PRIORITY_ACTION_ADD:
#if defined(RTEMS_SMP)
        _Priority_Insert(
          priority_aggregation,
          priority_action_node,
          &queue_context->Priority.Actions,
          _Thread_Priority_action_add,
          _Thread_Priority_action_change,
          the_thread
        );
#else
        _Priority_Non_empty_insert(
          priority_aggregation,
          priority_action_node,
          &queue_context->Priority.Actions,
          _Thread_Priority_action_change,
          NULL
        );
#endif
        break;
      case PRIORITY_ACTION_REMOVE:
#if defined(RTEMS_SMP)
        _Priority_Extract(
          priority_aggregation,
          priority_action_node,
          &queue_context->Priority.Actions,
          _Thread_Priority_action_remove,
          _Thread_Priority_action_change,
          the_thread
        );
#else
        _Priority_Extract_non_empty(
          priority_aggregation,
          priority_action_node,
          &queue_context->Priority.Actions,
          _Thread_Priority_action_change,
          NULL
        );
#endif
        break;
      default:
        _Assert( priority_action_type == PRIORITY_ACTION_CHANGE );
        _Priority_Changed(
          priority_aggregation,
          priority_action_node,
          priority_group_order,
          &queue_context->Priority.Actions,
          _Thread_Priority_action_change,
          NULL
        );
        break;
    }

#if defined(RTEMS_SMP)
    priority_aggregation = next_aggregation;
  } while ( priority_aggregation != NULL );
#else
  } while ( false );
#endif

  if ( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
    _Thread_queue_Context_add_priority_update( queue_context, the_thread );

    ( *operations->priority_actions )(
      queue,
      &queue_context->Priority.Actions
    );
  }
}

void _Thread_Priority_perform_actions(
  Thread_Control       *start_of_path,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *the_thread;
  size_t          update_count;

  _Assert( start_of_path != NULL );

  /*
   * This function is tricky on SMP configurations.  Please note that we do not
   * use the thread queue path available via the thread queue context.  Instead
   * we directly use the thread wait information to traverse the thread queue
   * path.  Thus, we do not necessarily acquire all thread queue locks on our
   * own.  In case of a deadlock, we use locks acquired by other processors
   * along the path.
   */

  the_thread = start_of_path;
  update_count = _Thread_queue_Context_get_priority_updates( queue_context );

  while ( true ) {
    Thread_queue_Queue *queue;

    queue = the_thread->Wait.queue;

    _Thread_Priority_do_perform_actions(
      the_thread,
      queue,
      the_thread->Wait.operations,
      PRIORITY_GROUP_LAST,
      queue_context
    );

    if ( _Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
      return;
    }

    _Assert( queue != NULL );
    the_thread = queue->owner;
    _Assert( the_thread != NULL );

    /*
     * In case the priority action list is non-empty, then the current thread
     * is enqueued on a thread queue.  There is no need to notify the scheduler
     * about a priority change, since it will pick up the new priority once it
     * is unblocked.  Restore the previous set of threads bound to update the
     * priority.
     */
    _Thread_queue_Context_restore_priority_updates(
      queue_context,
      update_count
    );
  }
}

static void _Thread_Priority_apply(
  Thread_Control       *the_thread,
  Priority_Node        *priority_action_node,
  Thread_queue_Context *queue_context,
  Priority_Group_order  priority_group_order,
  Priority_Action_type  priority_action_type
)
{
  Scheduler_Node     *scheduler_node;
  Thread_queue_Queue *queue;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  _Priority_Actions_initialize_one(
    &queue_context->Priority.Actions,
    &scheduler_node->Wait.Priority,
    priority_action_node,
    priority_action_type
  );
  queue = the_thread->Wait.queue;
  _Thread_Priority_do_perform_actions(
    the_thread,
    queue,
    the_thread->Wait.operations,
    priority_group_order,
    queue_context
  );

  if ( !_Priority_Actions_is_empty( &queue_context->Priority.Actions ) ) {
#if defined(RTEMS_SMP)
    (void) _Thread_queue_Path_acquire( queue, the_thread, queue_context );
#endif
    _Thread_Priority_perform_actions( queue->owner, queue_context );
#if defined(RTEMS_SMP)
    _Thread_queue_Path_release( queue_context );
#endif
  }
}

void _Thread_Priority_add(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  _Thread_Priority_apply(
    the_thread,
    priority_node,
    queue_context,
    PRIORITY_GROUP_LAST,
    PRIORITY_ACTION_ADD
  );
}

void _Thread_Priority_remove(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  _Thread_Priority_apply(
    the_thread,
    priority_node,
    queue_context,
    PRIORITY_GROUP_FIRST,
    PRIORITY_ACTION_REMOVE
  );
}

void _Thread_Priority_changed(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Priority_Group_order  priority_group_order,
  Thread_queue_Context *queue_context
)
{
  _Thread_Priority_apply(
    the_thread,
    priority_node,
    queue_context,
    priority_group_order,
    PRIORITY_ACTION_CHANGE
  );
}

#if defined(RTEMS_SMP)
void _Thread_Priority_replace(
  Thread_Control *the_thread,
  Priority_Node  *victim_node,
  Priority_Node  *replacement_node
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  _Priority_Replace(
    &scheduler_node->Wait.Priority,
    victim_node,
    replacement_node
  );
}
#endif

void _Thread_Priority_update( Thread_queue_Context *queue_context )
{
  size_t i;
  size_t n;

  n = queue_context->Priority.update_count;

  /*
   * Update the priority of all threads of the set.  Do not care to clear the
   * set, since the thread queue context will soon get destroyed anyway.
   */
  for ( i = 0; i < n ; ++i ) {
    Thread_Control   *the_thread;
    ISR_lock_Context  lock_context;

    the_thread = queue_context->Priority.update[ i ];
    _Thread_State_acquire( the_thread, &lock_context );
    _Scheduler_Update_priority( the_thread );
    _Thread_State_release( the_thread, &lock_context );
  }
}

#if defined(RTEMS_SMP)
static void _Thread_Priority_update_helping(
  Thread_Control *the_thread,
  Chain_Node     *first_node
)
{
  const Chain_Node *tail;
  Chain_Node       *node;

  tail = _Chain_Immutable_tail( &the_thread->Scheduler.Scheduler_nodes );
  node = _Chain_Next( first_node );

  while ( node != tail ) {
    Scheduler_Node          *scheduler_node;
    const Scheduler_Control *scheduler;
    ISR_lock_Context         lock_context;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.update_priority )(
      scheduler,
      the_thread,
      scheduler_node
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    node = _Chain_Next( node );
  }
}

void _Thread_Priority_update_and_make_sticky( Thread_Control *the_thread )
{
  ISR_lock_Context         lock_context;
  ISR_lock_Context         lock_context_2;
  Chain_Node              *node;
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  int                      new_sticky_level;
  int                      make_sticky_level;

  _Thread_State_acquire( the_thread, &lock_context );
  _Thread_Scheduler_process_requests( the_thread );

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

  _Scheduler_Acquire_critical( scheduler, &lock_context_2 );

  new_sticky_level = scheduler_node->sticky_level + 1;
  scheduler_node->sticky_level = new_sticky_level;
  _Assert( new_sticky_level >= 1 );

  /*
   * The sticky level is incremented by the scheduler block operation, so for a
   * ready thread, the change to sticky happens at a level of two.
   */
  make_sticky_level = 1 + (int) _Thread_Is_ready( the_thread );

  if ( new_sticky_level == make_sticky_level ) {
    ( *scheduler->Operations.make_sticky )(
      scheduler,
      the_thread,
      scheduler_node
    );
  }

  ( *scheduler->Operations.update_priority )(
    scheduler,
    the_thread,
    scheduler_node
  );

  _Scheduler_Release_critical( scheduler, &lock_context_2 );
  _Thread_Priority_update_helping( the_thread, node );
  _Thread_State_release( the_thread, &lock_context );
}

void _Thread_Priority_update_and_clean_sticky( Thread_Control *the_thread )
{
  ISR_lock_Context         lock_context;
  ISR_lock_Context         lock_context_2;
  Chain_Node              *node;
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  int                      new_sticky_level;
  int                      clean_sticky_level;

  _Thread_State_acquire( the_thread, &lock_context );
  _Thread_Scheduler_process_requests( the_thread );

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

  _Scheduler_Acquire_critical( scheduler, &lock_context_2 );

  new_sticky_level = scheduler_node->sticky_level - 1;
  scheduler_node->sticky_level = new_sticky_level;
  _Assert( new_sticky_level >= 0 );

  /*
   * The sticky level is incremented by the scheduler block operation, so for a
   * ready thread, the change to sticky happens at a level of one.
   */
  clean_sticky_level = (int) _Thread_Is_ready( the_thread );

  if ( new_sticky_level == clean_sticky_level ) {
    ( *scheduler->Operations.clean_sticky )(
      scheduler,
      the_thread,
      scheduler_node
    );
  }

  ( *scheduler->Operations.update_priority )(
    scheduler,
    the_thread,
    scheduler_node
  );

  _Scheduler_Release_critical( scheduler, &lock_context_2 );
  _Thread_Priority_update_helping( the_thread, node );
  _Thread_State_release( the_thread, &lock_context );
}

void _Thread_Priority_update_ignore_sticky( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;

  _Thread_State_acquire( the_thread, &lock_context );
  _Thread_Scheduler_process_requests( the_thread );
  _Scheduler_Update_priority( the_thread );
  _Thread_State_release( the_thread, &lock_context );
}
#endif
