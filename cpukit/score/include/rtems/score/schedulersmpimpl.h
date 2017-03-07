/**
 * @file
 *
 * @brief SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_SCHEDULERSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERSMPIMPL_H

#include <rtems/score/schedulersmp.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreSchedulerSMP
 *
 * The scheduler nodes can be in four states
 * - @ref SCHEDULER_SMP_NODE_BLOCKED,
 * - @ref SCHEDULER_SMP_NODE_SCHEDULED, and
 * - @ref SCHEDULER_SMP_NODE_READY.
 *
 * State transitions are triggered via basic operations
 * - _Scheduler_SMP_Enqueue_ordered(),
 * - _Scheduler_SMP_Enqueue_scheduled_ordered(), and
 * - _Scheduler_SMP_Block().
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *
 *   bs [label="BLOCKED"];
 *   ss [label="SCHEDULED", fillcolor="green"];
 *   rs [label="READY", fillcolor="red"];
 *
 *   edge [label="enqueue"];
 *   edge [fontcolor="darkgreen", color="darkgreen"];
 *
 *   bs -> ss;
 *
 *   edge [fontcolor="red", color="red"];
 *
 *   bs -> rs;
 *
 *   edge [label="enqueue other"];
 *
 *   ss -> rs;
 *
 *   edge [label="block"];
 *   edge [fontcolor="black", color="black"];
 *
 *   ss -> bs;
 *   rs -> bs;
 *
 *   edge [label="block other"];
 *   edge [fontcolor="darkgreen", color="darkgreen"];
 *
 *   rs -> ss;
 * }
 * @enddot
 *
 * During system initialization each processor of the scheduler instance starts
 * with an idle thread assigned to it.  Lets have a look at an example with two
 * idle threads I and J with priority 5.  We also have blocked threads A, B and
 * C with priorities 1, 2 and 3 respectively.  The scheduler nodes are ordered
 * with respect to the thread priority from left to right in the below
 * diagrams.  The highest priority node (lowest priority number) is the
 * leftmost node.  Since the processor assignment is independent of the thread
 * priority the processor indices may move from one state to the other.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *   subgraph {
 *     rank = same;
 *
 *     i [label="I (5)", fillcolor="green"];
 *     j [label="J (5)", fillcolor="green"];
 *     a [label="A (1)"];
 *     b [label="B (2)"];
 *     c [label="C (3)"];
 *     i -> j;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   i -> p0;
 *   j -> p1;
 * }
 * @enddot
 *
 * Lets start A.  For this an enqueue operation is performed.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *
 *   subgraph {
 *     rank = same;
 *
 *     i [label="I (5)", fillcolor="green"];
 *     j [label="J (5)", fillcolor="red"];
 *     a [label="A (1)", fillcolor="green"];
 *     b [label="B (2)"];
 *     c [label="C (3)"];
 *     a -> i;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   i -> p0;
 *   a -> p1;
 * }
 * @enddot
 *
 * Lets start C.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *
 *   subgraph {
 *     rank = same;
 *
 *     a [label="A (1)", fillcolor="green"];
 *     c [label="C (3)", fillcolor="green"];
 *     i [label="I (5)", fillcolor="red"];
 *     j [label="J (5)", fillcolor="red"];
 *     b [label="B (2)"];
 *     a -> c;
 *     i -> j;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   c -> p0;
 *   a -> p1;
 * }
 * @enddot
 *
 * Lets start B.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *
 *   subgraph {
 *     rank = same;
 *
 *     a [label="A (1)", fillcolor="green"];
 *     b [label="B (2)", fillcolor="green"];
 *     c [label="C (3)", fillcolor="red"];
 *     i [label="I (5)", fillcolor="red"];
 *     j [label="J (5)", fillcolor="red"];
 *     a -> b;
 *     c -> i -> j;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   b -> p0;
 *   a -> p1;
 * }
 * @enddot
 *
 * Lets change the priority of thread A to 4.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *
 *   subgraph {
 *     rank = same;
 *
 *     b [label="B (2)", fillcolor="green"];
 *     c [label="C (3)", fillcolor="green"];
 *     a [label="A (4)", fillcolor="red"];
 *     i [label="I (5)", fillcolor="red"];
 *     j [label="J (5)", fillcolor="red"];
 *     b -> c;
 *     a -> i -> j;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   b -> p0;
 *   c -> p1;
 * }
 * @enddot
 *
 * Now perform a blocking operation with thread B.  Please note that thread A
 * migrated now from processor 0 to processor 1 and thread C still executes on
 * processor 1.
 *
 * @dot
 * digraph {
 *   node [style="filled"];
 *   edge [dir="none"];
 *
 *   subgraph {
 *     rank = same;
 *
 *     c [label="C (3)", fillcolor="green"];
 *     a [label="A (4)", fillcolor="green"];
 *     i [label="I (5)", fillcolor="red"];
 *     j [label="J (5)", fillcolor="red"];
 *     b [label="B (2)"];
 *     c -> a;
 *     i -> j;
 *   }
 *
 *   subgraph {
 *     rank = same;
 *
 *     p0 [label="PROCESSOR 0", shape="box"];
 *     p1 [label="PROCESSOR 1", shape="box"];
 *   }
 *
 *   a -> p0;
 *   c -> p1;
 * }
 * @enddot
 *
 * @{
 */

typedef bool ( *Scheduler_SMP_Has_ready )(
  Scheduler_Context *context
);

typedef Scheduler_Node *( *Scheduler_SMP_Get_highest_ready )(
  Scheduler_Context *context,
  Scheduler_Node    *node
);

typedef Scheduler_Node *( *Scheduler_SMP_Get_lowest_scheduled )(
  Scheduler_Context *context,
  Scheduler_Node    *filter,
  Chain_Node_order   order
);

typedef void ( *Scheduler_SMP_Extract )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
);

typedef void ( *Scheduler_SMP_Insert )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert
);

typedef void ( *Scheduler_SMP_Move )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_move
);

typedef bool ( *Scheduler_SMP_Ask_for_help )(
  Scheduler_Context *context,
  Thread_Control    *thread,
  Scheduler_Node    *node
);

typedef void ( *Scheduler_SMP_Update )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_update,
  Priority_Control   new_priority
);

typedef bool ( *Scheduler_SMP_Enqueue )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_enqueue
);

typedef void ( *Scheduler_SMP_Allocate_processor )(
  Scheduler_Context *context,
  Thread_Control    *scheduled_thread,
  Thread_Control    *victim_thread,
  Per_CPU_Control   *victim_cpu
);

static inline bool _Scheduler_SMP_Insert_priority_lifo_order(
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const Scheduler_SMP_Node *node_to_insert =
    (const Scheduler_SMP_Node *) to_insert;
  const Scheduler_SMP_Node *node_next =
    (const Scheduler_SMP_Node *) next;

  return node_to_insert->priority <= node_next->priority;
}

static inline bool _Scheduler_SMP_Insert_priority_fifo_order(
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const Scheduler_SMP_Node *node_to_insert =
    (const Scheduler_SMP_Node *) to_insert;
  const Scheduler_SMP_Node *node_next =
    (const Scheduler_SMP_Node *) next;

  return node_to_insert->priority < node_next->priority;
}

static inline Scheduler_SMP_Context *_Scheduler_SMP_Get_self(
  Scheduler_Context *context
)
{
  return (Scheduler_SMP_Context *) context;
}

static inline void _Scheduler_SMP_Initialize(
  Scheduler_SMP_Context *self
)
{
  _Chain_Initialize_empty( &self->Scheduled );
  _Chain_Initialize_empty( &self->Idle_threads );
}

static inline Scheduler_SMP_Node *_Scheduler_SMP_Thread_get_node(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Thread_Scheduler_get_home_node( thread );
}

static inline Scheduler_SMP_Node *_Scheduler_SMP_Thread_get_own_node(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Thread_Scheduler_get_home_node( thread );
}

static inline Scheduler_SMP_Node *_Scheduler_SMP_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_SMP_Node *) node;
}

static inline Scheduler_SMP_Node_state _Scheduler_SMP_Node_state(
  const Scheduler_Node *node
)
{
  return ( (const Scheduler_SMP_Node *) node )->state;
}

static inline Priority_Control _Scheduler_SMP_Node_priority(
  const Scheduler_Node *node
)
{
  return ( (const Scheduler_SMP_Node *) node )->priority;
}

static inline void _Scheduler_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_SMP_Node      *node,
  Thread_Control          *thread,
  Priority_Control         priority
)
{
  _Scheduler_Node_do_initialize( scheduler, &node->Base, thread, priority );
  node->state = SCHEDULER_SMP_NODE_BLOCKED;
  node->priority = priority;
}

static inline void _Scheduler_SMP_Node_update_priority(
  Scheduler_SMP_Node *node,
  Priority_Control    new_priority
)
{
  node->priority = new_priority;
}

static inline void _Scheduler_SMP_Node_change_state(
  Scheduler_Node           *node,
  Scheduler_SMP_Node_state  new_state
)
{
  Scheduler_SMP_Node *the_node;

  the_node = _Scheduler_SMP_Node_downcast( node );
  the_node->state = new_state;
}

static inline bool _Scheduler_SMP_Is_processor_owned_by_us(
  const Scheduler_Context *context,
  const Per_CPU_Control   *cpu
)
{
  return cpu->Scheduler.context == context;
}

static inline Thread_Control *_Scheduler_SMP_Get_idle_thread(
  Scheduler_Context *context
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Thread_Control *idle = (Thread_Control *)
    _Chain_Get_first_unprotected( &self->Idle_threads );

  _Assert( &idle->Object.Node != _Chain_Tail( &self->Idle_threads ) );

  return idle;
}

static inline void _Scheduler_SMP_Release_idle_thread(
  Scheduler_Context *context,
  Thread_Control    *idle
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );

  _Chain_Prepend_unprotected( &self->Idle_threads, &idle->Object.Node );
}

static inline void _Scheduler_SMP_Exctract_idle_thread(
  Thread_Control *idle
)
{
  _Chain_Extract_unprotected( &idle->Object.Node );
}

static inline void _Scheduler_SMP_Allocate_processor_lazy(
  Scheduler_Context *context,
  Thread_Control    *scheduled_thread,
  Thread_Control    *victim_thread,
  Per_CPU_Control   *victim_cpu
)
{
  Per_CPU_Control *scheduled_cpu = _Thread_Get_CPU( scheduled_thread );
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  Thread_Control *heir;

  _Assert( _ISR_Get_level() != 0 );

  if ( _Thread_Is_executing_on_a_processor( scheduled_thread ) ) {
    if ( _Scheduler_SMP_Is_processor_owned_by_us( context, scheduled_cpu ) ) {
      heir = scheduled_cpu->heir;
      _Thread_Dispatch_update_heir(
        cpu_self,
        scheduled_cpu,
        scheduled_thread
      );
    } else {
      /* We have to force a migration to our processor set */
      heir = scheduled_thread;
    }
  } else {
    heir = scheduled_thread;
  }

  if ( heir != victim_thread ) {
    _Thread_Set_CPU( heir, victim_cpu );
    _Thread_Dispatch_update_heir( cpu_self, victim_cpu, heir );
  }
}

/*
 * This method is slightly different from
 * _Scheduler_SMP_Allocate_processor_lazy() in that it does what it is asked to
 * do.  _Scheduler_SMP_Allocate_processor_lazy() attempts to prevent migrations
 * but does not take into account affinity.
 */
static inline void _Scheduler_SMP_Allocate_processor_exact(
  Scheduler_Context *context,
  Thread_Control    *scheduled_thread,
  Thread_Control    *victim_thread,
  Per_CPU_Control   *victim_cpu
)
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  (void) context;

  _Thread_Set_CPU( scheduled_thread, victim_cpu );
  _Thread_Dispatch_update_heir( cpu_self, victim_cpu, scheduled_thread );
}

static inline void _Scheduler_SMP_Allocate_processor(
  Scheduler_Context                *context,
  Scheduler_Node                   *scheduled,
  Thread_Control                   *victim_thread,
  Per_CPU_Control                  *victim_cpu,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Thread_Control *scheduled_thread = _Scheduler_Node_get_user( scheduled );

  _Scheduler_SMP_Node_change_state( scheduled, SCHEDULER_SMP_NODE_SCHEDULED );

  ( *allocate_processor )(
    context,
    scheduled_thread,
    victim_thread,
    victim_cpu
  );
}

static inline Thread_Control *_Scheduler_SMP_Preempt(
  Scheduler_Context                *context,
  Scheduler_Node                   *scheduled,
  Scheduler_Node                   *victim,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Thread_Control   *victim_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *victim_cpu;

  victim_thread = _Scheduler_Node_get_user( victim );
  _Scheduler_SMP_Node_change_state( victim, SCHEDULER_SMP_NODE_READY );

  _Thread_Scheduler_acquire_critical( victim_thread, &lock_context );

  victim_cpu = _Thread_Get_CPU( victim_thread );

  if ( victim_thread->Scheduler.state == THREAD_SCHEDULER_SCHEDULED ) {
    _Scheduler_Thread_change_state( victim_thread, THREAD_SCHEDULER_READY );

    if ( victim_thread->Scheduler.helping_nodes > 0 ) {
      _Per_CPU_Acquire( victim_cpu );
      _Chain_Append_unprotected(
        &victim_cpu->Threads_in_need_for_help,
        &victim_thread->Scheduler.Help_node
      );
      _Per_CPU_Release( victim_cpu );
    }
  }

  _Thread_Scheduler_release_critical( victim_thread, &lock_context );

  _Scheduler_SMP_Allocate_processor(
    context,
    scheduled,
    victim_thread,
    victim_cpu,
    allocate_processor
  );

  return victim_thread;
}

static inline Scheduler_Node *_Scheduler_SMP_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter,
  Chain_Node_order   order
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Chain_Control *scheduled = &self->Scheduled;
  Scheduler_Node *lowest_scheduled =
    (Scheduler_Node *) _Chain_Last( scheduled );

  (void) filter;
  (void) order;

  _Assert( &lowest_scheduled->Node != _Chain_Tail( scheduled ) );
  _Assert(
    _Chain_Next( &lowest_scheduled->Node ) == _Chain_Tail( scheduled )
  );

  return lowest_scheduled;
}

static inline void _Scheduler_SMP_Enqueue_to_scheduled(
  Scheduler_Context                *context,
  Scheduler_Node                   *node,
  Scheduler_Node                   *lowest_scheduled,
  Scheduler_SMP_Insert              insert_scheduled,
  Scheduler_SMP_Move                move_from_scheduled_to_ready,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Scheduler_Try_to_schedule_action action;

  action = _Scheduler_Try_to_schedule_node(
    context,
    node,
    _Scheduler_Node_get_idle( lowest_scheduled ),
    _Scheduler_SMP_Get_idle_thread
  );

  if ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE ) {
    _Scheduler_SMP_Preempt(
      context,
      node,
      lowest_scheduled,
      allocate_processor
    );

    ( *insert_scheduled )( context, node );
    ( *move_from_scheduled_to_ready )( context, lowest_scheduled );

    _Scheduler_Release_idle_thread(
      context,
      lowest_scheduled,
      _Scheduler_SMP_Release_idle_thread
    );
  } else if ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE ) {
    _Scheduler_SMP_Node_change_state(
      lowest_scheduled,
      SCHEDULER_SMP_NODE_READY
    );
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_SCHEDULED );

    ( *insert_scheduled )( context, node );
    ( *move_from_scheduled_to_ready )( context, lowest_scheduled );

    _Scheduler_Exchange_idle_thread(
      node,
      lowest_scheduled,
      _Scheduler_Node_get_idle( lowest_scheduled )
    );
  } else {
    _Assert( action == SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK );
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );
  }
}

/**
 * @brief Enqueues a node according to the specified order function.
 *
 * The node must not be in the scheduled state.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The node to enqueue.
 * @param[in] order The order function.
 * @param[in] insert_ready Function to insert a node into the set of ready
 *   nodes.
 * @param[in] insert_scheduled Function to insert a node into the set of
 *   scheduled nodes.
 * @param[in] move_from_scheduled_to_ready Function to move a node from the set
 *   of scheduled nodes to the set of ready nodes.
 * @param[in] get_lowest_scheduled Function to select the node from the
 *   scheduled nodes to replace.  It may not be possible to find one, in this
 *   case a pointer must be returned so that the order functions returns false
 *   if this pointer is passed as the second argument to the order function.
 * @param[in] allocate_processor Function to allocate a processor to a node
 *   based on the rules of the scheduler.
 */
static inline bool _Scheduler_SMP_Enqueue_ordered(
  Scheduler_Context                  *context,
  Scheduler_Node                     *node,
  Chain_Node_order                    order,
  Scheduler_SMP_Insert                insert_ready,
  Scheduler_SMP_Insert                insert_scheduled,
  Scheduler_SMP_Move                  move_from_scheduled_to_ready,
  Scheduler_SMP_Get_lowest_scheduled  get_lowest_scheduled,
  Scheduler_SMP_Allocate_processor    allocate_processor
)
{
  bool            needs_help;
  Scheduler_Node *lowest_scheduled;

  lowest_scheduled = ( *get_lowest_scheduled )( context, node, order );

  if ( ( *order )( &node->Node, &lowest_scheduled->Node ) ) {
    _Scheduler_SMP_Enqueue_to_scheduled(
      context,
      node,
      lowest_scheduled,
      insert_scheduled,
      move_from_scheduled_to_ready,
      allocate_processor
    );
    needs_help = false;
  } else {
    ( *insert_ready )( context, node );
    needs_help = true;
  }

  return needs_help;
}

/**
 * @brief Enqueues a scheduled node according to the specified order
 * function.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The node to enqueue.
 * @param[in] order The order function.
 * @param[in] extract_from_ready Function to extract a node from the set of
 *   ready nodes.
 * @param[in] get_highest_ready Function to get the highest ready node.
 * @param[in] insert_ready Function to insert a node into the set of ready
 *   nodes.
 * @param[in] insert_scheduled Function to insert a node into the set of
 *   scheduled nodes.
 * @param[in] move_from_ready_to_scheduled Function to move a node from the set
 *   of ready nodes to the set of scheduled nodes.
 * @param[in] allocate_processor Function to allocate a processor to a node
 *   based on the rules of the scheduler.
 */
static inline bool _Scheduler_SMP_Enqueue_scheduled_ordered(
  Scheduler_Context                *context,
  Scheduler_Node                   *node,
  Chain_Node_order                  order,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Insert              insert_ready,
  Scheduler_SMP_Insert              insert_scheduled,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  while ( true ) {
    Scheduler_Node                   *highest_ready;
    Scheduler_Try_to_schedule_action  action;

    highest_ready = ( *get_highest_ready )( context, node );

    /*
     * The node has been extracted from the scheduled chain.  We have to place
     * it now on the scheduled or ready set.
     */
    if (
      node->sticky_level > 0
        && ( *order )( &node->Node, &highest_ready->Node )
    ) {
      ( *insert_scheduled )( context, node );

      if ( _Scheduler_Node_get_idle( node ) != NULL ) {
        Thread_Control   *owner;
        ISR_lock_Context  lock_context;

        owner = _Scheduler_Node_get_owner( node );
        _Thread_Scheduler_acquire_critical( owner, &lock_context );

        if ( owner->Scheduler.state == THREAD_SCHEDULER_READY ) {
          _Thread_Scheduler_cancel_need_for_help(
            owner,
            _Thread_Get_CPU( owner )
          );
          _Scheduler_Discard_idle_thread(
            context,
            owner,
            node,
            _Scheduler_SMP_Release_idle_thread
          );
          _Scheduler_Thread_change_state( owner, THREAD_SCHEDULER_SCHEDULED );
        }

        _Thread_Scheduler_release_critical( owner, &lock_context );
      }

      return false;
    }

    action = _Scheduler_Try_to_schedule_node(
      context,
      highest_ready,
      _Scheduler_Node_get_idle( node ),
      _Scheduler_SMP_Get_idle_thread
    );

    if ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE ) {
      Thread_Control *idle;

      _Scheduler_SMP_Preempt(
        context,
        highest_ready,
        node,
        allocate_processor
      );

      ( *insert_ready )( context, node );
      ( *move_from_ready_to_scheduled )( context, highest_ready );

      idle = _Scheduler_Release_idle_thread(
        context,
        node,
        _Scheduler_SMP_Release_idle_thread
      );
      return ( idle == NULL );
    } else if ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE ) {
      _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
      _Scheduler_SMP_Node_change_state(
        highest_ready,
        SCHEDULER_SMP_NODE_SCHEDULED
      );

      ( *insert_ready )( context, node );
      ( *move_from_ready_to_scheduled )( context, highest_ready );

      _Scheduler_Exchange_idle_thread(
        highest_ready,
        node,
        _Scheduler_Node_get_idle( node )
      );
      return false;
    } else {
      _Assert( action == SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK );

      _Scheduler_SMP_Node_change_state(
        highest_ready,
        SCHEDULER_SMP_NODE_BLOCKED
      );

      ( *extract_from_ready )( context, highest_ready );
    }
  }
}

static inline void _Scheduler_SMP_Extract_from_scheduled(
  Scheduler_Node *node
)
{
  _Chain_Extract_unprotected( &node->Node );
}

static inline void _Scheduler_SMP_Schedule_highest_ready(
  Scheduler_Context                *context,
  Scheduler_Node                   *victim,
  Per_CPU_Control                  *victim_cpu,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Scheduler_Try_to_schedule_action action;

  do {
    Scheduler_Node *highest_ready = ( *get_highest_ready )( context, victim );

    action = _Scheduler_Try_to_schedule_node(
      context,
      highest_ready,
      NULL,
      _Scheduler_SMP_Get_idle_thread
    );

    if ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE ) {
      _Scheduler_SMP_Allocate_processor(
        context,
        highest_ready,
        _Scheduler_Node_get_user( victim ),
        victim_cpu,
        allocate_processor
      );

      ( *move_from_ready_to_scheduled )( context, highest_ready );
    } else {
      _Assert( action == SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK );

      _Scheduler_SMP_Node_change_state(
        highest_ready,
        SCHEDULER_SMP_NODE_BLOCKED
      );

      ( *extract_from_ready )( context, highest_ready );
    }
  } while ( action == SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK );
}

/**
 * @brief Blocks a thread.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] thread The thread of the scheduling operation.
 * @param[in] node The scheduler node of the thread to block.
 * @param[in] extract_from_ready Function to extract a node from the set of
 *   ready nodes.
 * @param[in] get_highest_ready Function to get the highest ready node.
 * @param[in] move_from_ready_to_scheduled Function to move a node from the set
 *   of ready nodes to the set of scheduled nodes.
 */
static inline void _Scheduler_SMP_Block(
  Scheduler_Context                *context,
  Thread_Control                   *thread,
  Scheduler_Node                   *node,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Scheduler_SMP_Node_state  node_state;
  Per_CPU_Control          *thread_cpu;

  node_state = _Scheduler_SMP_Node_state( node );

  thread_cpu = _Scheduler_Block_node(
    context,
    thread,
    node,
    node_state == SCHEDULER_SMP_NODE_SCHEDULED,
    _Scheduler_SMP_Get_idle_thread
  );

  if ( thread_cpu != NULL ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );

    if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
      _Scheduler_SMP_Extract_from_scheduled( node );
      _Scheduler_SMP_Schedule_highest_ready(
        context,
        node,
        thread_cpu,
        extract_from_ready,
        get_highest_ready,
        move_from_ready_to_scheduled,
        allocate_processor
      );
    } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
      ( *extract_from_ready )( context, node );
    }
  }
}

static inline void _Scheduler_SMP_Unblock(
  Scheduler_Context     *context,
  Thread_Control        *thread,
  Scheduler_Node        *node,
  Scheduler_SMP_Update   update,
  Scheduler_SMP_Enqueue  enqueue_fifo
)
{
  Scheduler_SMP_Node_state  node_state;
  bool                      unblock;

  node_state = _Scheduler_SMP_Node_state( node );
  unblock = _Scheduler_Unblock_node(
    context,
    thread,
    node,
    node_state == SCHEDULER_SMP_NODE_SCHEDULED,
    _Scheduler_SMP_Release_idle_thread
  );

  if ( unblock ) {
    Priority_Control new_priority;
    bool             prepend_it;
    bool             needs_help;

    new_priority = _Scheduler_Node_get_priority( node, &prepend_it );
    (void) prepend_it;

    if ( new_priority != _Scheduler_SMP_Node_priority( node ) ) {
      ( *update )( context, node, new_priority );
    }

    if ( node_state == SCHEDULER_SMP_NODE_BLOCKED ) {
      _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );

      needs_help = ( *enqueue_fifo )( context, node );
    } else {
      _Assert( node_state == SCHEDULER_SMP_NODE_READY );
      _Assert( node->sticky_level > 0 );
      _Assert( node->idle == NULL );
      needs_help = true;
    }

    if ( needs_help ) {
      _Scheduler_Ask_for_help( thread );
    }
  }
}

static inline void _Scheduler_SMP_Update_priority(
  Scheduler_Context          *context,
  Thread_Control             *thread,
  Scheduler_Node             *node,
  Scheduler_SMP_Extract       extract_from_ready,
  Scheduler_SMP_Update        update,
  Scheduler_SMP_Enqueue       enqueue_fifo,
  Scheduler_SMP_Enqueue       enqueue_lifo,
  Scheduler_SMP_Enqueue       enqueue_scheduled_fifo,
  Scheduler_SMP_Enqueue       enqueue_scheduled_lifo,
  Scheduler_SMP_Ask_for_help  ask_for_help
)
{
  Priority_Control         new_priority;
  bool                     prepend_it;
  Scheduler_SMP_Node_state node_state;

  new_priority = _Scheduler_Node_get_priority( node, &prepend_it );

  if ( new_priority == _Scheduler_SMP_Node_priority( node ) ) {
    if ( _Thread_Is_ready( thread ) ) {
      ( *ask_for_help )( context, thread, node );
    }

    return;
  }

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    _Scheduler_SMP_Extract_from_scheduled( node );

    ( *update )( context, node, new_priority );

    if ( prepend_it ) {
      ( *enqueue_scheduled_lifo )( context, node );
    } else {
      ( *enqueue_scheduled_fifo )( context, node );
    }
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    ( *extract_from_ready )( context, node );

    ( *update )( context, node, new_priority );

    if ( prepend_it ) {
      ( *enqueue_lifo )( context, node );
    } else {
      ( *enqueue_fifo )( context, node );
    }
  } else {
    ( *update )( context, node, new_priority );

    if ( _Thread_Is_ready( thread ) ) {
      ( *ask_for_help )( context, thread, node );
    }
  }
}

static inline void _Scheduler_SMP_Yield(
  Scheduler_Context     *context,
  Thread_Control        *thread,
  Scheduler_Node        *node,
  Scheduler_SMP_Extract  extract_from_ready,
  Scheduler_SMP_Enqueue  enqueue_fifo,
  Scheduler_SMP_Enqueue  enqueue_scheduled_fifo
)
{
  bool                     needs_help;
  Scheduler_SMP_Node_state node_state;

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    _Scheduler_SMP_Extract_from_scheduled( node );
    ( *enqueue_scheduled_fifo )( context, node );
    needs_help = false;
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    ( *extract_from_ready )( context, node );

    needs_help = ( *enqueue_fifo )( context, node );
  } else {
    needs_help = true;
  }

  if ( needs_help ) {
    _Scheduler_Ask_for_help( thread );
  }
}

static inline void _Scheduler_SMP_Insert_scheduled_lifo(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &node_to_insert->Node,
    _Scheduler_SMP_Insert_priority_lifo_order
  );
}

static inline void _Scheduler_SMP_Insert_scheduled_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &node_to_insert->Node,
    _Scheduler_SMP_Insert_priority_fifo_order
  );
}

static inline bool _Scheduler_SMP_Ask_for_help(
  Scheduler_Context                  *context,
  Thread_Control                     *thread,
  Scheduler_Node                     *node,
  Chain_Node_order                    order,
  Scheduler_SMP_Insert                insert_ready,
  Scheduler_SMP_Insert                insert_scheduled,
  Scheduler_SMP_Move                  move_from_scheduled_to_ready,
  Scheduler_SMP_Get_lowest_scheduled  get_lowest_scheduled,
  Scheduler_SMP_Allocate_processor    allocate_processor
)
{
  Scheduler_Node   *lowest_scheduled;
  ISR_lock_Context  lock_context;
  bool              success;

  lowest_scheduled = ( *get_lowest_scheduled )( context, node, order );

  _Thread_Scheduler_acquire_critical( thread, &lock_context );

  if ( thread->Scheduler.state == THREAD_SCHEDULER_READY ) {
    Scheduler_SMP_Node_state node_state;

    node_state = _Scheduler_SMP_Node_state( node );

    if ( node_state == SCHEDULER_SMP_NODE_BLOCKED ) {
      if ( ( *order )( &node->Node, &lowest_scheduled->Node ) ) {
        _Thread_Scheduler_cancel_need_for_help(
          thread,
          _Thread_Get_CPU( thread )
        );
        _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_SCHEDULED );
        _Thread_Scheduler_release_critical( thread, &lock_context );

        _Scheduler_SMP_Preempt(
          context,
          node,
          lowest_scheduled,
          allocate_processor
        );

        ( *insert_scheduled )( context, node );
        ( *move_from_scheduled_to_ready )( context, lowest_scheduled );

        _Scheduler_Release_idle_thread(
          context,
          lowest_scheduled,
          _Scheduler_SMP_Release_idle_thread
        );
        success = true;
      } else {
        _Thread_Scheduler_release_critical( thread, &lock_context );
        _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
        ( *insert_ready )( context, node );
        success = false;
      }
    } else if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
      _Thread_Scheduler_cancel_need_for_help(
        thread,
        _Thread_Get_CPU( thread )
      );
      _Scheduler_Discard_idle_thread(
        context,
        thread,
        node,
        _Scheduler_SMP_Release_idle_thread
      );
      _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_SCHEDULED );
      _Thread_Scheduler_release_critical( thread, &lock_context );
      success = true;
    } else {
      _Thread_Scheduler_release_critical( thread, &lock_context );
      success = false;
    }
  } else {
    _Thread_Scheduler_release_critical( thread, &lock_context );
    success = false;
  }

  return success;
}

static inline void _Scheduler_SMP_Reconsider_help_request(
  Scheduler_Context     *context,
  Thread_Control        *thread,
  Scheduler_Node        *node,
  Scheduler_SMP_Extract  extract_from_ready
)
{
  ISR_lock_Context lock_context;

  _Thread_Scheduler_acquire_critical( thread, &lock_context );

  if (
    thread->Scheduler.state == THREAD_SCHEDULER_SCHEDULED
      && _Scheduler_SMP_Node_state( node ) == SCHEDULER_SMP_NODE_READY
      && node->sticky_level == 1
  ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );
    ( *extract_from_ready )( context, node );
  }

  _Thread_Scheduler_release_critical( thread, &lock_context );
}

static inline void _Scheduler_SMP_Withdraw_node(
  Scheduler_Context                *context,
  Thread_Control                   *thread,
  Scheduler_Node                   *node,
  Thread_Scheduler_state            next_state,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  ISR_lock_Context         lock_context;
  Scheduler_SMP_Node_state node_state;

  _Thread_Scheduler_acquire_critical( thread, &lock_context );

  node_state = _Scheduler_SMP_Node_state( node );
  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    Per_CPU_Control *thread_cpu;

    thread_cpu = _Thread_Get_CPU( thread );
    _Scheduler_Thread_change_state( thread, next_state );
    _Thread_Scheduler_release_critical( thread, &lock_context );

    _Scheduler_SMP_Extract_from_scheduled( node );
    _Scheduler_SMP_Schedule_highest_ready(
      context,
      node,
      thread_cpu,
      extract_from_ready,
      get_highest_ready,
      move_from_ready_to_scheduled,
      allocate_processor
    );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    _Thread_Scheduler_release_critical( thread, &lock_context );
    ( *extract_from_ready )( context, node );
  } else {
    _Assert( node_state == SCHEDULER_SMP_NODE_BLOCKED );
    _Thread_Scheduler_release_critical( thread, &lock_context );
  }
}

static inline void _Scheduler_SMP_Add_processor(
  Scheduler_Context       *context,
  Thread_Control          *idle,
  Scheduler_SMP_Has_ready  has_ready,
  Scheduler_SMP_Enqueue    enqueue_scheduled_fifo
)
{
  Scheduler_SMP_Context *self;
  Scheduler_Node        *node;

  self = _Scheduler_SMP_Get_self( context );
  idle->Scheduler.state = THREAD_SCHEDULER_SCHEDULED;
  _Scheduler_SMP_Release_idle_thread( &self->Base, idle );
  node = _Thread_Scheduler_get_home_node( idle );
  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_SCHEDULED );

  if ( ( *has_ready )( &self->Base ) ) {
    ( *enqueue_scheduled_fifo )( &self->Base, node );
  } else {
    _Chain_Append_unprotected( &self->Scheduled, &node->Node );
  }
}

static inline Thread_Control *_Scheduler_SMP_Remove_processor(
  Scheduler_Context     *context,
  Per_CPU_Control       *cpu,
  Scheduler_SMP_Extract  extract_from_ready,
  Scheduler_SMP_Enqueue  enqueue_fifo
)
{
  Scheduler_SMP_Context *self;
  Chain_Node            *chain_node;
  Scheduler_Node        *victim_node;
  Thread_Control        *victim_user;
  Thread_Control        *victim_owner;
  Thread_Control        *idle;

  self = _Scheduler_SMP_Get_self( context );
  chain_node = _Chain_First( &self->Scheduled );

  do {
    _Assert( chain_node != _Chain_Immutable_tail( &self->Scheduled ) );
    victim_node = (Scheduler_Node *) chain_node;
    victim_user = _Scheduler_Node_get_user( victim_node );
    chain_node = _Chain_Next( chain_node );
  } while ( _Thread_Get_CPU( victim_user ) != cpu );

  _Scheduler_SMP_Extract_from_scheduled( victim_node );
  victim_owner = _Scheduler_Node_get_owner( victim_node );

  if ( !victim_owner->is_idle ) {
    Scheduler_Node *idle_node;

    _Scheduler_Release_idle_thread(
      &self->Base,
      victim_node,
      _Scheduler_SMP_Release_idle_thread
    );
    idle = _Scheduler_SMP_Get_idle_thread( &self->Base );
    idle_node = _Thread_Scheduler_get_home_node( idle );
    ( *extract_from_ready )( &self->Base, idle_node );
    _Scheduler_SMP_Preempt(
      &self->Base,
      idle_node,
      victim_node,
      _Scheduler_SMP_Allocate_processor_exact
    );

    if ( !_Chain_Is_empty( &self->Scheduled ) ) {
      ( *enqueue_fifo )( context, victim_node );
    }
  } else {
    _Assert( victim_owner == victim_user );
    _Assert( _Scheduler_Node_get_idle( victim_node ) == NULL );
    idle = victim_owner;
    _Scheduler_SMP_Exctract_idle_thread( idle );
  }

  return idle;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
