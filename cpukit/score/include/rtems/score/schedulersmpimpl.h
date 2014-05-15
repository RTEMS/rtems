/**
 * @file
 *
 * @brief SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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
 * C with priorities 1, 2 and 3 respectively.
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

typedef Thread_Control *( *Scheduler_SMP_Get_highest_ready )(
  Scheduler_Context *context
);

typedef void ( *Scheduler_SMP_Extract )(
  Scheduler_Context *context,
  Thread_Control *thread
);

typedef void ( *Scheduler_SMP_Insert )(
  Scheduler_Context *context,
  Thread_Control *thread_to_insert
);

typedef void ( *Scheduler_SMP_Move )(
  Scheduler_Context *context,
  Thread_Control *thread_to_move
);

typedef void ( *Scheduler_SMP_Update )(
  Scheduler_Context *context,
  Scheduler_Node *node,
  Priority_Control new_priority
);

typedef void ( *Scheduler_SMP_Enqueue )(
  Scheduler_Context *context,
  Thread_Control *thread_to_enqueue
);

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
}

static inline Scheduler_SMP_Node *_Scheduler_SMP_Node_get(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Scheduler_Node_get( thread );
}

static inline void _Scheduler_SMP_Node_initialize(
  Scheduler_SMP_Node *node
)
{
  node->state = SCHEDULER_SMP_NODE_BLOCKED;
}

extern const bool _Scheduler_SMP_Node_valid_state_changes[ 3 ][ 3 ];

static inline void _Scheduler_SMP_Node_change_state(
  Scheduler_SMP_Node *node,
  Scheduler_SMP_Node_state new_state
)
{
  _Assert(
    _Scheduler_SMP_Node_valid_state_changes[ node->state ][ new_state ]
  );

  node->state = new_state;
}

static inline bool _Scheduler_SMP_Is_processor_owned_by_us(
  const Scheduler_SMP_Context *self,
  const Per_CPU_Control *cpu
)
{
  return cpu->scheduler_context == &self->Base;
}

static inline void _Scheduler_SMP_Update_heir(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_for_heir,
  Thread_Control *heir
)
{
  cpu_for_heir->heir = heir;

  /*
   * It is critical that we first update the heir and then the dispatch
   * necessary so that _Thread_Get_heir_and_make_it_executing() cannot miss an
   * update.
   */
  _Atomic_Fence( ATOMIC_ORDER_SEQ_CST );

  /*
   * Only update the dispatch necessary indicator if not already set to
   * avoid superfluous inter-processor interrupts.
   */
  if ( !cpu_for_heir->dispatch_necessary ) {
    cpu_for_heir->dispatch_necessary = true;

    if ( cpu_for_heir != cpu_self ) {
      _Per_CPU_Send_interrupt( cpu_for_heir );
    }
  }
}

static inline void _Scheduler_SMP_Allocate_processor(
  Scheduler_SMP_Context *self,
  Thread_Control *scheduled,
  Thread_Control *victim
)
{
  Scheduler_SMP_Node *scheduled_node = _Scheduler_SMP_Node_get( scheduled );
  Per_CPU_Control *cpu_of_scheduled = _Thread_Get_CPU( scheduled );
  Per_CPU_Control *cpu_of_victim = _Thread_Get_CPU( victim );
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  Thread_Control *heir;

  _Scheduler_SMP_Node_change_state(
    scheduled_node,
    SCHEDULER_SMP_NODE_SCHEDULED
  );

  _Assert( _ISR_Get_level() != 0 );

  if ( _Thread_Is_executing_on_a_processor( scheduled ) ) {
    if ( _Scheduler_SMP_Is_processor_owned_by_us( self, cpu_of_scheduled ) ) {
      heir = cpu_of_scheduled->heir;
      _Scheduler_SMP_Update_heir( cpu_self, cpu_of_scheduled, scheduled );
    } else {
      /* We have to force a migration to our processor set */
      _Assert( scheduled->debug_real_cpu->heir != scheduled );
      heir = scheduled;
    }
  } else {
    heir = scheduled;
  }

  if ( heir != victim ) {
    _Thread_Set_CPU( heir, cpu_of_victim );
    _Scheduler_SMP_Update_heir( cpu_self, cpu_of_victim, heir );
  }
}

static inline Thread_Control *_Scheduler_SMP_Get_lowest_scheduled(
  Scheduler_SMP_Context *self
)
{
  Thread_Control *lowest_ready = NULL;
  Chain_Control *scheduled = &self->Scheduled;

  if ( !_Chain_Is_empty( scheduled ) ) {
    lowest_ready = (Thread_Control *) _Chain_Last( scheduled );
  }

  return lowest_ready;
}

/**
 * @brief Enqueues a thread according to the specified order function.
 *
 * The thread must not be in the scheduled state.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] thread The thread to enqueue.
 * @param[in] order The order function.
 * @param[in] insert_ready Function to insert a node into the set of ready
 * nodes.
 * @param[in] insert_scheduled Function to insert a node into the set of
 * scheduled nodes.
 * @param[in] move_from_scheduled_to_ready Function to move a node from the set
 * of scheduled nodes to the set of ready nodes.
 */
static inline void _Scheduler_SMP_Enqueue_ordered(
  Scheduler_Context *context,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled,
  Scheduler_SMP_Move move_from_scheduled_to_ready
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Thread_Control *lowest_scheduled =
    _Scheduler_SMP_Get_lowest_scheduled( self );

  _Assert( lowest_scheduled != NULL);

  /*
   * NOTE: Do not exchange parameters to do the negation of the order check.
   */
  if ( ( *order )( &thread->Object.Node, &lowest_scheduled->Object.Node ) ) {
    Scheduler_SMP_Node *lowest_scheduled_node =
      _Scheduler_SMP_Node_get( lowest_scheduled );

    _Scheduler_SMP_Node_change_state(
      lowest_scheduled_node,
      SCHEDULER_SMP_NODE_READY
    );
    _Scheduler_SMP_Allocate_processor( self, thread, lowest_scheduled );
    ( *insert_scheduled )( &self->Base, thread );
    ( *move_from_scheduled_to_ready )( &self->Base, lowest_scheduled );
  } else {
    ( *insert_ready )( &self->Base, thread );
  }
}

/**
 * @brief Enqueues a scheduled thread according to the specified order
 * function.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] thread The thread to enqueue.
 * @param[in] order The order function.
 * @param[in] get_highest_ready Function to get the highest ready node.
 * @param[in] insert_ready Function to insert a node into the set of ready
 * nodes.
 * @param[in] insert_scheduled Function to insert a node into the set of
 * scheduled nodes.
 * @param[in] move_from_ready_to_scheduled Function to move a node from the set
 * of ready nodes to the set of scheduled nodes.
 */
static inline void _Scheduler_SMP_Enqueue_scheduled_ordered(
  Scheduler_Context *context,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );
  Thread_Control *highest_ready = ( *get_highest_ready )( &self->Base );

  _Assert( highest_ready != NULL);

  /*
   * The thread has been extracted from the scheduled chain.  We have to place
   * it now on the scheduled or ready set.
   *
   * NOTE: Do not exchange parameters to do the negation of the order check.
   */
  if ( !( *order )( &thread->Object.Node, &highest_ready->Object.Node ) ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
    _Scheduler_SMP_Allocate_processor( self, highest_ready, thread );
    ( *insert_ready )( &self->Base, thread );
    ( *move_from_ready_to_scheduled )( &self->Base, highest_ready );
  } else {
    ( *insert_scheduled )( &self->Base, thread );
  }
}

static inline void _Scheduler_SMP_Extract_from_scheduled(
  Thread_Control *thread
)
{
  _Chain_Extract_unprotected( &thread->Object.Node );
}

static inline void _Scheduler_SMP_Schedule_highest_ready(
  Scheduler_Context *context,
  Thread_Control *victim,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Thread_Control *highest_ready = ( *get_highest_ready )( &self->Base );

  _Scheduler_SMP_Allocate_processor( self, highest_ready, victim );

  ( *move_from_ready_to_scheduled )( &self->Base, highest_ready );
}

/**
 * @brief Blocks a thread.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] thread The thread of the scheduling operation.
 * @param[in] extract_from_ready Function to extract a node from the set of
 * ready nodes.
 * @param[in] get_highest_ready Function to get the highest ready node.
 * @param[in] move_from_ready_to_scheduled Function to move a node from the set
 * of ready nodes to the set of scheduled nodes.
 */
static inline void _Scheduler_SMP_Block(
  Scheduler_Context *context,
  Thread_Control *thread,
  Scheduler_SMP_Extract extract_from_ready,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );
  bool is_scheduled = node->state == SCHEDULER_SMP_NODE_SCHEDULED;

  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );

  if ( is_scheduled ) {
    _Scheduler_SMP_Extract_from_scheduled( thread );

    _Scheduler_SMP_Schedule_highest_ready(
      context,
      thread,
      get_highest_ready,
      move_from_ready_to_scheduled
    );
  } else {
    ( *extract_from_ready )( context, thread );
  }
}

static inline void _Scheduler_SMP_Unblock(
  Scheduler_Context *context,
  Thread_Control *thread,
  Scheduler_SMP_Enqueue enqueue_fifo
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );

  ( *enqueue_fifo )( context, thread );
}

static inline void _Scheduler_SMP_Change_priority(
  Scheduler_Context *context,
  Thread_Control *thread,
  Priority_Control new_priority,
  bool prepend_it,
  Scheduler_SMP_Extract extract_from_ready,
  Scheduler_SMP_Update update,
  Scheduler_SMP_Enqueue enqueue_fifo,
  Scheduler_SMP_Enqueue enqueue_lifo,
  Scheduler_SMP_Enqueue enqueue_scheduled_fifo,
  Scheduler_SMP_Enqueue enqueue_scheduled_lifo
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  if ( node->state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    _Scheduler_SMP_Extract_from_scheduled( thread );

    ( *update )( context, &node->Base, new_priority );

    if ( prepend_it ) {
      ( *enqueue_scheduled_lifo )( context, thread );
    } else {
      ( *enqueue_scheduled_fifo )( context, thread );
    }
  } else {
    ( *extract_from_ready )( context, thread );

    ( *update )( context, &node->Base, new_priority );

    if ( prepend_it ) {
      ( *enqueue_lifo )( context, thread );
    } else {
      ( *enqueue_fifo )( context, thread );
    }
  }
}

static inline void _Scheduler_SMP_Insert_scheduled_lifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static inline void _Scheduler_SMP_Insert_scheduled_fifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
