/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerSMP which are only used by the implementation.
 */

/*
 * Copyright (C) 2013, 2021 embedded brains GmbH & Co. KG
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
 * @addtogroup RTEMSScoreSchedulerSMP
 *
 * The scheduler nodes can be in four states
 * - @ref SCHEDULER_SMP_NODE_BLOCKED,
 * - @ref SCHEDULER_SMP_NODE_SCHEDULED, and
 * - @ref SCHEDULER_SMP_NODE_READY.
 *
 * State transitions are triggered via basic operations
 * - _Scheduler_SMP_Enqueue(),
 * - _Scheduler_SMP_Enqueue_scheduled(), and
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
  Scheduler_Node    *filter
);

typedef Scheduler_Node *( *Scheduler_SMP_Get_lowest_ready )(
  Scheduler_Context *context
);

typedef Scheduler_Node *( *Scheduler_SMP_Get_lowest_scheduled )(
  Scheduler_Context *context,
  Scheduler_Node    *filter
);

typedef void ( *Scheduler_SMP_Extract )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
);

typedef void ( *Scheduler_SMP_Insert )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert,
  Priority_Control   insert_priority
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

typedef void ( *Scheduler_SMP_Set_affinity )(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  void              *arg
);

typedef bool ( *Scheduler_SMP_Enqueue )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_enqueue,
  Priority_Control   priority
);

typedef void ( *Scheduler_SMP_Enqueue_scheduled )(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_enqueue,
  Priority_Control   priority
);

typedef void ( *Scheduler_SMP_Allocate_processor )(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled,
  Per_CPU_Control   *cpu
);

typedef void ( *Scheduler_SMP_Register_idle )(
  Scheduler_Context *context,
  Scheduler_Node    *idle,
  Per_CPU_Control   *cpu
);

/**
 * @brief Does nothing.
 *
 * @param context This parameter is unused.
 * @param idle This parameter is unused.
 * @param cpu This parameter is unused.
 */
static inline void _Scheduler_SMP_Do_nothing_register_idle(
  Scheduler_Context *context,
  Scheduler_Node    *idle,
  Per_CPU_Control   *cpu
)
{
  (void) context;
  (void) idle;
  (void) cpu;
}

/**
 * @brief Checks if @a to_insert is less or equal than the priority of the chain node.
 *
 * @param key is the priority to compare.
 *
 * @param to_insert is the chain node to insert.
 *
 * @param next is the chain node to compare the priority of.
 *
 * @retval true @a to_insert is less or equal than the priority of @a next.
 * @retval false @a to_insert is greater than the priority of @a next.
 */
static inline bool _Scheduler_SMP_Priority_less_equal(
  const void       *key,
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const Priority_Control   *priority_to_insert;
  const Scheduler_SMP_Node *node_next;

  (void) to_insert;
  priority_to_insert = (const Priority_Control *) key;
  node_next = (const Scheduler_SMP_Node *) next;

  return *priority_to_insert <= node_next->priority;
}

/**
 * @brief Gets the scheduler smp context.
 *
 * @param context The context to cast to Scheduler_SMP_Context *.
 *
 * @return @a context cast to Scheduler_SMP_Context *.
 */
static inline Scheduler_SMP_Context *_Scheduler_SMP_Get_self(
  Scheduler_Context *context
)
{
  return (Scheduler_SMP_Context *) context;
}

/**
 * @brief Initializes the scheduler smp context.
 *
 * @param[out] self The context to initialize.
 */
static inline void _Scheduler_SMP_Initialize(
  Scheduler_SMP_Context *self
)
{
  _Chain_Initialize_empty( &self->Scheduled );
}

/**
 * @brief Gets the scheduler smp node of the thread.
 *
 * @param thread The thread to get the smp node of.
 *
 * @return The scheduler smp node of @a thread.
 */
static inline Scheduler_SMP_Node *_Scheduler_SMP_Thread_get_node(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Thread_Scheduler_get_home_node( thread );
}

/**
 * @brief Gets the scheduler smp node of the thread.
 *
 * @param thread The thread to get the smp node of.
 *
 * @return The scheduler smp node of @a thread.
 */
static inline Scheduler_SMP_Node *_Scheduler_SMP_Thread_get_own_node(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Thread_Scheduler_get_home_node( thread );
}

/**
 * @brief Gets the scheduler smp node.
 *
 * @param node The node to cast to Scheduler_SMP_Node *.
 *
 * @return @a node cast to Scheduler_SMP_Node *.
 */
static inline Scheduler_SMP_Node *_Scheduler_SMP_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_SMP_Node *) node;
}

/**
 * @brief Gets the state of the node.
 *
 * @param node The node to get the state of.
 *
 * @return The state of @a node.
 */
static inline Scheduler_SMP_Node_state _Scheduler_SMP_Node_state(
  const Scheduler_Node *node
)
{
  return ( (const Scheduler_SMP_Node *) node )->state;
}

/**
 * @brief Gets the priority of the node.
 *
 * @param node The node to get the priority of.
 *
 * @return The priority of @a node.
 */
static inline Priority_Control _Scheduler_SMP_Node_priority(
  const Scheduler_Node *node
)
{
  return ( (const Scheduler_SMP_Node *) node )->priority;
}

/**
 * @brief Initializes the scheduler smp node.
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The node to initialize.
 * @param thread The thread of the scheduler smp node.
 * @param priority The priority to initialize @a node with.
 */
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

/**
 * @brief Updates the priority of the node to the new priority.
 *
 * @param[out] node The node to update the priority of.
 * @param new_priority The new priority for @a node.
 */
static inline void _Scheduler_SMP_Node_update_priority(
  Scheduler_SMP_Node *node,
  Priority_Control    new_priority
)
{
  node->priority = new_priority;
}

/**
 * @brief Changes the state of the node to the given state.
 *
 * @param[out] node the node to change the state of.
 * @param new_state The new state for @a node.
 */
static inline void _Scheduler_SMP_Node_change_state(
  Scheduler_Node           *node,
  Scheduler_SMP_Node_state  new_state
)
{
  Scheduler_SMP_Node *the_node;

  the_node = _Scheduler_SMP_Node_downcast( node );
  the_node->state = new_state;
}

/**
 * @brief Checks if the processor is owned by the given context.
 *
 * @param context The context to check whether @a cpu is owned by it.
 * @param cpu The cpu to check whether it is owned by @a context.
 *
 * @retval true @a cpu is owned by @a context.
 * @retval false @a cpu is not owned by @a context.
 */
static inline bool _Scheduler_SMP_Is_processor_owned_by_us(
  const Scheduler_Context *context,
  const Per_CPU_Control   *cpu
)
{
  return cpu->Scheduler.context == context;
}

/**
 * @brief Removes the thread's ask for help request from the processor.
 *
 * The caller must be the owner of the thread's scheduler lock.
 *
 * @param[in, out] thread is the thread of the ask for help request.
 *
 * @param[in, out] cpu is the processor from which the ask for help request
 *   should be removed.
 */
void _Scheduler_SMP_Remove_ask_for_help_from_processor(
  Thread_Control  *thread,
  Per_CPU_Control *cpu
);

/**
 * @brief Cancels the thread's ask for help request.
 *
 * The caller must be the owner of the thread's scheduler lock.
 *
 * @param[in, out] thread is the thread of the ask help request.
 */
static inline void _Scheduler_SMP_Cancel_ask_for_help( Thread_Control *thread )
{
  Per_CPU_Control *cpu;

  _Assert( _ISR_lock_Is_owner( &thread->Scheduler.Lock ) );
  cpu = thread->Scheduler.ask_for_help_cpu;

  if ( RTEMS_PREDICT_FALSE( cpu != NULL ) ) {
    _Scheduler_SMP_Remove_ask_for_help_from_processor( thread, cpu );
  }
}

/**
 * @brief Requests to ask for help for the thread.
 *
 * The actual ask for help operations are carried out during
 * _Thread_Do_dispatch() on the current processor.
 *
 * An alternative approach would be to carry out the requests on a processor
 * related to the thread.  This could reduce the overhead for the preempting
 * thread a bit, however, there are at least two problems with this approach.
 * Firstly, we have to figure out what is a processor related to the thread.
 * Secondly, we may need an inter-processor interrupt.
 *
 * @param[in, out] thread is the thread in need for help.
 */
static inline void _Scheduler_SMP_Request_ask_for_help( Thread_Control *thread )
{
  ISR_lock_Context lock_context;
  Per_CPU_Control *cpu_self;

  cpu_self = _Per_CPU_Get();

  _Assert( thread->Scheduler.ask_for_help_cpu == NULL );
  thread->Scheduler.ask_for_help_cpu = cpu_self;
  cpu_self->dispatch_necessary = true;

  _Per_CPU_Acquire( cpu_self, &lock_context );
  _Chain_Append_unprotected(
    &cpu_self->Threads_in_need_for_help,
    &thread->Scheduler.Help_node
  );
  _Per_CPU_Release( cpu_self, &lock_context );
}

/**
 * @brief This enumeration defines what a scheduler should do with a node which
 * could be scheduled.
 */
typedef enum {
  SCHEDULER_SMP_DO_SCHEDULE,
  SCHEDULER_SMP_DO_NOT_SCHEDULE
} Scheduler_SMP_Action;

/**
 * @brief Tries to schedule the scheduler node.
 *
 * When an SMP scheduler needs to schedule a node, it shall use this function
 * to determine what it shall do with the node.
 *
 * This function uses the state of the node and the scheduler state of the
 * owner thread to determine what shall be done.  Each scheduler maintains its
 * nodes independent of other schedulers.  This function ensures that a thread
 * is scheduled by at most one scheduler.  If a node requires an executing
 * thread due to some locking protocol and the owner thread is already
 * scheduled by another scheduler, then an idle thread will be attached to the
 * node.
 *
 * @param[in, out] node is the node which should be scheduled.
 *
 * @param get_idle_node is the get idle node handler.
 *
 * @param arg is the get idle node handler argument.
 *
 * @retval SCHEDULER_SMP_DO_SCHEDULE The node shall be scheduled.
 *
 * @retval SCHEDULER_SMP_DO_NOT_SCHEDULE The node shall be blocked.  This
 *   action is returned, if the owner thread is already scheduled by another
 *   scheduler.
 */
static inline Scheduler_SMP_Action _Scheduler_SMP_Try_to_schedule(
  Scheduler_Node          *node,
  Scheduler_Get_idle_node  get_idle_node,
  void                    *arg
)
{
  ISR_lock_Context        lock_context;
  Thread_Control         *owner;
  Thread_Scheduler_state  owner_state;
  int                     owner_sticky_level;

  owner = _Scheduler_Node_get_owner( node );
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );

  _Thread_Scheduler_acquire_critical( owner, &lock_context );
  owner_state = owner->Scheduler.state;
  owner_sticky_level = node->sticky_level;

  if ( RTEMS_PREDICT_TRUE( owner_state == THREAD_SCHEDULER_READY ) ) {
    _Scheduler_SMP_Cancel_ask_for_help( owner );
    _Scheduler_Thread_change_state( owner, THREAD_SCHEDULER_SCHEDULED );
    _Thread_Scheduler_release_critical( owner, &lock_context );
    return SCHEDULER_SMP_DO_SCHEDULE;
  }

  _Thread_Scheduler_release_critical( owner, &lock_context );

  if (
    ( owner_state == THREAD_SCHEDULER_SCHEDULED && owner_sticky_level <= 1 ) ||
    owner_sticky_level == 0
  ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );

    return SCHEDULER_SMP_DO_NOT_SCHEDULE;
  }

  (void) _Scheduler_Use_idle_thread( node, get_idle_node, arg );

  return SCHEDULER_SMP_DO_SCHEDULE;
}

/**
 * @brief Allocates a processor to the user of the scheduled node.
 *
 * Attempts to prevent migrations but does not take into account affinity.
 *
 * @param[in, out] context is the scheduler context.
 *
 * @param[in, out] scheduled is the scheduled node that gets the processor allocated.
 *
 * @param[in, out] cpu is the processor to allocate.
 */
static inline void _Scheduler_SMP_Allocate_processor_lazy(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled,
  Per_CPU_Control   *cpu
)
{
  Thread_Control *scheduled_thread = _Scheduler_Node_get_user( scheduled );
  Per_CPU_Control *scheduled_cpu = _Thread_Get_CPU( scheduled_thread );
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  _Assert( _ISR_Get_level() != 0 );

  if ( cpu == scheduled_cpu ) {
    _Thread_Set_CPU( scheduled_thread, cpu );
    _Thread_Dispatch_update_heir( cpu_self, cpu, scheduled_thread );

    return;
  }

  if (
    _Thread_Is_executing_on_a_processor( scheduled_thread ) &&
    _Scheduler_SMP_Is_processor_owned_by_us( context, scheduled_cpu )
  ) {
    Thread_Control *heir = scheduled_cpu->heir;
    _Thread_Dispatch_update_heir( cpu_self, scheduled_cpu, scheduled_thread );
    _Thread_Set_CPU( heir, cpu );
    _Thread_Dispatch_update_heir( cpu_self, cpu, heir );

    return;
  }

  _Thread_Set_CPU( scheduled_thread, cpu );
  _Thread_Dispatch_update_heir( cpu_self, cpu, scheduled_thread );
}

/**
 * @brief Allocates exactly the processor to the user of the scheduled node.
 *
 * This method is slightly different from
 * _Scheduler_SMP_Allocate_processor_lazy() in that it does what it is asked to
 * do.  _Scheduler_SMP_Allocate_processor_lazy() attempts to prevent migrations
 * but does not take into account affinity.
 *
 * @param[in, out] context is the scheduler context.
 *
 * @param[in, out] scheduled is the scheduled node that gets the processor allocated.
 *
 * @param[in, out] cpu is the processor to allocate.
 */
static inline void _Scheduler_SMP_Allocate_processor_exact(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled,
  Per_CPU_Control   *cpu
)
{
  Thread_Control *scheduled_thread = _Scheduler_Node_get_user( scheduled );
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  (void) context;

  _Thread_Set_CPU( scheduled_thread, cpu );
  _Thread_Dispatch_update_heir( cpu_self, cpu, scheduled_thread );
}

/**
 * @brief Allocates the processor to the user of the scheduled node using the
 *   given allocation handler.
 *
 * @param[in, out] context is the scheduler context.
 *
 * @param[in, out] scheduled is the scheduled node that gets the processor allocated.
 *
 * @param[in, out] cpu is the processor to allocate.
 *
 * @param allocate_processor is the handler which should allocate the processor.
 */
static inline void _Scheduler_SMP_Allocate_processor(
  Scheduler_Context                *context,
  Scheduler_Node                   *scheduled,
  Per_CPU_Control                  *cpu,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  _Scheduler_SMP_Node_change_state( scheduled, SCHEDULER_SMP_NODE_SCHEDULED );
  ( *allocate_processor )( context, scheduled, cpu );
}

/**
 * @brief Preempts the victim's thread and allocates a processor for the user
 *   of the scheduled node.
 *
 * @param[in, out] context is the scheduler context.
 *
 * @param scheduled[in, out] is the node of the user thread that is about to
 *   get a processor allocated.
 *
 * @param[in, out] victim is the victim node of the thread to preempt.
 *
 * @param[in, out] victim_idle is the idle thread used by the victim node or NULL.
 *
 * @param allocate_processor The function for allocation of a processor for the new thread.
 */
static inline void _Scheduler_SMP_Preempt(
  Scheduler_Context                *context,
  Scheduler_Node                   *scheduled,
  Scheduler_Node                   *victim,
  Thread_Control                   *victim_idle,
  Scheduler_SMP_Allocate_processor  allocate_processor
)
{
  Thread_Control   *victim_owner;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu;

  _Scheduler_SMP_Node_change_state( victim, SCHEDULER_SMP_NODE_READY );

  victim_owner = _Scheduler_Node_get_owner( victim );
  _Thread_Scheduler_acquire_critical( victim_owner, &lock_context );

  if ( RTEMS_PREDICT_TRUE( victim_idle == NULL ) ) {
    if ( victim_owner->Scheduler.state == THREAD_SCHEDULER_SCHEDULED ) {
      _Scheduler_Thread_change_state( victim_owner, THREAD_SCHEDULER_READY );

      if ( victim_owner->Scheduler.helping_nodes > 0 ) {
        _Scheduler_SMP_Request_ask_for_help( victim_owner );
      }
    }

    cpu = _Thread_Get_CPU( victim_owner );
  } else {
    cpu = _Thread_Get_CPU( victim_idle );
  }

  _Thread_Scheduler_release_critical( victim_owner, &lock_context );

  _Scheduler_SMP_Allocate_processor(
    context,
    scheduled,
    cpu,
    allocate_processor
  );
}

/**
 * @brief Returns the lowest member of the scheduled nodes.
 *
 * @param context The scheduler context instance.
 * @param filter This parameter is unused.
 *
 * @return The lowest scheduled node.
 */
static inline Scheduler_Node *_Scheduler_SMP_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter
)
{
  Scheduler_SMP_Context *self;
  Scheduler_Node        *lowest_scheduled;

  (void) filter;

  self = _Scheduler_SMP_Get_self( context );

  _Assert( !_Chain_Is_empty( &self->Scheduled ) );
  lowest_scheduled = (Scheduler_Node *) _Chain_Last( &self->Scheduled );

  _Assert(
    _Chain_Next( &lowest_scheduled->Node.Chain ) ==
      _Chain_Tail( &self->Scheduled )
  );

  return lowest_scheduled;
}

/**
 * @brief Tries to schedule the given node.
 *
 * Schedules the node, or blocks if that is necessary.
 *
 * @param context The scheduler context instance.
 * @param[in, out] node The node to insert into the scheduled nodes.
 * @param priority The priority of @a node.
 * @param[in, out] lowest_scheduled The lowest member of the scheduled nodes.
 * @param insert_scheduled Function to insert a node into the set of
 *   scheduled nodes.
 * @param move_from_scheduled_to_ready Function to move a node from the set
 *   of scheduled nodes to the set of ready nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *   based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Enqueue_to_scheduled(
  Scheduler_Context                *context,
  Scheduler_Node                   *node,
  Priority_Control                  priority,
  Scheduler_Node                   *lowest_scheduled,
  Scheduler_SMP_Insert              insert_scheduled,
  Scheduler_SMP_Move                move_from_scheduled_to_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node,
  Scheduler_Release_idle_node       release_idle_node
)
{
  Thread_Control      *lowest_scheduled_idle;
  Scheduler_SMP_Action action;

  lowest_scheduled_idle = _Scheduler_Release_idle_thread_if_necessary(
    lowest_scheduled,
    release_idle_node,
    context
  );

  ( *move_from_scheduled_to_ready )( context, lowest_scheduled );

  action = _Scheduler_SMP_Try_to_schedule( node, get_idle_node, context );

  if ( RTEMS_PREDICT_TRUE( action == SCHEDULER_SMP_DO_SCHEDULE ) ) {
    _Scheduler_SMP_Preempt(
      context,
      node,
      lowest_scheduled,
      lowest_scheduled_idle,
      allocate_processor
    );

    ( *insert_scheduled )( context, node, priority );
  } else {
    _Assert( action == SCHEDULER_SMP_DO_NOT_SCHEDULE );

    if ( lowest_scheduled_idle != NULL ) {
      (void) _Scheduler_Use_idle_thread( lowest_scheduled, get_idle_node, context );
    }

    ( *move_from_ready_to_scheduled )( context, lowest_scheduled );
  }
}

/**
 * @brief Enqueues a node according to the specified order function.
 *
 * The node must not be in the scheduled state.
 *
 * @param context The scheduler instance context.
 * @param[in, out] node The node to enqueue.
 * @param priority The node insert priority.
 * @param order The order function.
 * @param insert_ready Function to insert a node into the set of ready
 *   nodes.
 * @param insert_scheduled Function to insert a node into the set of
 *   scheduled nodes.
 * @param move_from_scheduled_to_ready Function to move a node from the set
 *   of scheduled nodes to the set of ready nodes.
 * @param get_lowest_scheduled Function to select the node from the
 *   scheduled nodes to replace.  It may not be possible to find one, in this
 *   case a pointer must be returned so that the order functions returns false
 *   if this pointer is passed as the second argument to the order function.
 * @param allocate_processor Function to allocate a processor to a node
 *   based on the rules of the scheduler.
 */
static inline bool _Scheduler_SMP_Enqueue(
  Scheduler_Context                  *context,
  Scheduler_Node                     *node,
  Priority_Control                    insert_priority,
  Chain_Node_order                    order,
  Scheduler_SMP_Insert                insert_ready,
  Scheduler_SMP_Insert                insert_scheduled,
  Scheduler_SMP_Move                  move_from_scheduled_to_ready,
  Scheduler_SMP_Move                  move_from_ready_to_scheduled,
  Scheduler_SMP_Get_lowest_scheduled  get_lowest_scheduled,
  Scheduler_SMP_Allocate_processor    allocate_processor,
  Scheduler_Get_idle_node             get_idle_node,
  Scheduler_Release_idle_node         release_idle_node
)
{
  bool            needs_help;
  Scheduler_Node *lowest_scheduled;

  lowest_scheduled = ( *get_lowest_scheduled )( context, node );

  if (
    ( *order )(
      &insert_priority,
      &node->Node.Chain,
      &lowest_scheduled->Node.Chain
    )
  ) {
    _Scheduler_SMP_Enqueue_to_scheduled(
      context,
      node,
      insert_priority,
      lowest_scheduled,
      insert_scheduled,
      move_from_scheduled_to_ready,
      move_from_ready_to_scheduled,
      allocate_processor,
      get_idle_node,
      release_idle_node
    );
    needs_help = false;
  } else {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
    ( *insert_ready )( context, node, insert_priority );
    needs_help = true;
  }

  return needs_help;
}

/**
 * @brief Enqueues a scheduled node according to the specified order
 * function.
 *
 * @param context The scheduler instance context.
 * @param[in, out] node The node to enqueue.
 * @param order The order function.
 * @param extract_from_ready Function to extract a node from the set of
 *   ready nodes.
 * @param get_highest_ready Function to get the highest ready node.
 * @param insert_ready Function to insert a node into the set of ready
 *   nodes.
 * @param insert_scheduled Function to insert a node into the set of
 *   scheduled nodes.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *   of ready nodes to the set of scheduled nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *   based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Enqueue_scheduled(
  Scheduler_Context                *context,
  Scheduler_Node                   *const node,
  Priority_Control                  insert_priority,
  Chain_Node_order                  order,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Insert              insert_ready,
  Scheduler_SMP_Insert              insert_scheduled,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node,
  Scheduler_Release_idle_node       release_idle_node
)
{
  Thread_Control *node_idle;

  node_idle = _Scheduler_Release_idle_thread_if_necessary(
    node,
    release_idle_node,
    context
  );

  while ( true ) {
    Scheduler_Node       *highest_ready;
    Scheduler_SMP_Action  action;

    highest_ready = ( *get_highest_ready )( context, node );

    /*
     * The node has been extracted from the scheduled chain.  We have to place
     * it now on the scheduled or ready set.
     */
    if (
      node->sticky_level > 0 && ( *order )(
        &insert_priority,
        &node->Node.Chain,
        &highest_ready->Node.Chain
      )
    ) {
      if ( node_idle != NULL ) {
        Thread_Control   *owner;
        ISR_lock_Context  lock_context;

        owner = _Scheduler_Node_get_owner( node );
        _Thread_Scheduler_acquire_critical( owner, &lock_context );

        if ( owner->Scheduler.state == THREAD_SCHEDULER_READY ) {
          Per_CPU_Control *cpu;

          _Scheduler_SMP_Cancel_ask_for_help( owner );
          _Scheduler_Thread_change_state( owner, THREAD_SCHEDULER_SCHEDULED );
          cpu = _Thread_Get_CPU( node_idle );
          _Thread_Set_CPU( owner, cpu );
          _Thread_Scheduler_release_critical( owner, &lock_context );
          _Thread_Dispatch_update_heir( _Per_CPU_Get(), cpu, owner );
        } else {
          Thread_Control *new_idle;

          _Thread_Scheduler_release_critical( owner, &lock_context );
          new_idle = _Scheduler_Use_idle_thread( node, get_idle_node, context );
          _Assert_Unused_variable_equals( new_idle, node_idle );
        }
      }

      ( *insert_scheduled )( context, node, insert_priority );

      return;
    }

    action = _Scheduler_SMP_Try_to_schedule(
      highest_ready,
      get_idle_node,
      context
    );

    if ( RTEMS_PREDICT_TRUE( action == SCHEDULER_SMP_DO_SCHEDULE ) ) {
      _Scheduler_SMP_Preempt(
        context,
        highest_ready,
        node,
        node_idle,
        allocate_processor
      );

      ( *move_from_ready_to_scheduled )( context, highest_ready );
      ( *insert_ready )( context, node, insert_priority );
      return;
    }

    _Assert( action == SCHEDULER_SMP_DO_NOT_SCHEDULE );
    ( *extract_from_ready )( context, highest_ready );
  }
}

/**
 * @brief Extracts a scheduled node from the scheduled nodes.
 *
 * @param context This parameter is unused.
 * @param node The node to extract from the chain it belongs to.
 */
static inline void _Scheduler_SMP_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  (void) context;
  _Chain_Extract_unprotected( &node->Node.Chain );
}

/**
 * @brief Schedules the highest ready node.
 *
 * @param context The scheduler context instance.
 * @param victim The node of the thread that is repressed by the newly scheduled thread.
 * @param cpu is the processor to allocate.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the set of
 *      ready nodes.
 * @param get_highest_ready Function to get the highest ready node.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *      of ready nodes to the set of scheduled nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Schedule_highest_ready(
  Scheduler_Context                *context,
  Scheduler_Node                   *victim,
  Per_CPU_Control                  *cpu,
  Scheduler_SMP_Extract             extract_from_scheduled,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node
)
{
  Scheduler_SMP_Action action;

  _Scheduler_SMP_Node_change_state( victim, SCHEDULER_SMP_NODE_BLOCKED );
  ( *extract_from_scheduled )( context, victim );

  while ( true ) {
    Scheduler_Node *highest_ready = ( *get_highest_ready )( context, victim );

    action = _Scheduler_SMP_Try_to_schedule(
      highest_ready,
      get_idle_node,
      context
    );

    if ( RTEMS_PREDICT_TRUE( action == SCHEDULER_SMP_DO_SCHEDULE ) ) {
      _Scheduler_SMP_Allocate_processor(
        context,
        highest_ready,
        cpu,
        allocate_processor
      );

      ( *move_from_ready_to_scheduled )( context, highest_ready );
      return;
    }

    _Assert( action == SCHEDULER_SMP_DO_NOT_SCHEDULE );
    ( *extract_from_ready )( context, highest_ready );
  }
}

/**
 * @brief Schedules the highest ready node and preempts a currently executing one.
 *
 * @param context The scheduler context instance.
 * @param victim The node of the thread that is repressed by the newly scheduled thread.
 * @param extract_from_ready Function to extract a node from the set of
 *      ready nodes.
 * @param get_highest_ready Function to get the highest ready node.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *      of ready nodes to the set of scheduled nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Preempt_and_schedule_highest_ready(
  Scheduler_Context                *context,
  Scheduler_Node                   *victim,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node,
  Scheduler_Release_idle_node       release_idle_node
)
{
  Thread_Control      *victim_idle;
  Scheduler_SMP_Action action;

  _Scheduler_SMP_Node_change_state( victim, SCHEDULER_SMP_NODE_READY );
  victim_idle = _Scheduler_Release_idle_thread_if_necessary(
    victim,
    release_idle_node,
    context
  );

  while ( true ) {
    Scheduler_Node *highest_ready = ( *get_highest_ready )( context, victim );

    action = _Scheduler_SMP_Try_to_schedule(
      highest_ready,
      get_idle_node,
      context
    );

    if ( RTEMS_PREDICT_TRUE( action == SCHEDULER_SMP_DO_SCHEDULE ) ) {
      _Scheduler_SMP_Preempt(
        context,
        highest_ready,
        victim,
        victim_idle,
        allocate_processor
      );

      ( *move_from_ready_to_scheduled )( context, highest_ready );
      return;
    }

    _Assert( action == SCHEDULER_SMP_DO_NOT_SCHEDULE );
    ( *extract_from_ready )( context, highest_ready );
  }
}

/**
 * @brief Blocks the thread.
 *
 * @param context The scheduler instance context.
 * @param[in, out] thread The thread of the scheduling operation.
 * @param[in, out] node The scheduler node of the thread to block.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the set of
 *      ready nodes.
 * @param get_highest_ready Function to get the highest ready node.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *      of ready nodes to the set of scheduled nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Block(
  Scheduler_Context                *context,
  Thread_Control                   *thread,
  Scheduler_Node                   *node,
  Scheduler_SMP_Extract             extract_from_scheduled,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node
)
{
  int                       sticky_level;
  ISR_lock_Context          lock_context;
  Scheduler_SMP_Node_state  node_state;
  Per_CPU_Control          *cpu;

  sticky_level = node->sticky_level;
  --sticky_level;
  node->sticky_level = sticky_level;
  _Assert( sticky_level >= 0 );

  _Thread_Scheduler_acquire_critical( thread, &lock_context );
  _Scheduler_SMP_Cancel_ask_for_help( thread );
  cpu = _Thread_Get_CPU( thread );
  _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_BLOCKED );
  _Thread_Scheduler_release_critical( thread, &lock_context );

  node_state = _Scheduler_SMP_Node_state( node );

  if ( RTEMS_PREDICT_FALSE( sticky_level > 0 ) ) {
    if (
      node_state == SCHEDULER_SMP_NODE_SCHEDULED &&
      _Scheduler_Node_get_idle( node ) == NULL
    ) {
      Thread_Control *idle;

      idle = _Scheduler_Use_idle_thread( node, get_idle_node, context );
      _Thread_Set_CPU( idle, cpu );
      _Thread_Dispatch_update_heir( _Per_CPU_Get(), cpu, idle );
    }

    return;
  }

  _Assert( _Scheduler_Node_get_user( node ) == thread );
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    _Scheduler_SMP_Schedule_highest_ready(
      context,
      node,
      cpu,
      extract_from_scheduled,
      extract_from_ready,
      get_highest_ready,
      move_from_ready_to_scheduled,
      allocate_processor,
      get_idle_node
    );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );
    ( *extract_from_ready )( context, node );
  }
}

/**
 * @brief Unblocks the thread.
 *
 * @param context The scheduler instance context.
 * @param[in, out] thread The thread of the scheduling operation.
 * @param[in, out] node The scheduler node of the thread to block.
 * @param update Function to update the node's priority to the new value.
 * @param enqueue Function to insert a node with a priority in the ready queue
 *      of a context.
 */
static inline void _Scheduler_SMP_Unblock(
  Scheduler_Context          *context,
  Thread_Control             *thread,
  Scheduler_Node             *node,
  Scheduler_SMP_Update        update,
  Scheduler_SMP_Enqueue       enqueue,
  Scheduler_Release_idle_node release_idle_node
)
{
  Scheduler_SMP_Node_state  node_state;
  Priority_Control          priority;

  _Assert( _Chain_Is_node_off_chain( &thread->Scheduler.Help_node ) );

  ++node->sticky_level;
  _Assert( node->sticky_level > 0 );

  node_state = _Scheduler_SMP_Node_state( node );

  if ( RTEMS_PREDICT_FALSE( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) ) {
    _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_SCHEDULED );
    _Scheduler_Discard_idle_thread(
      thread,
      node,
      release_idle_node,
      context
    );

    return;
  }

  _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_READY );

  priority = _Scheduler_Node_get_priority( node );
  priority = SCHEDULER_PRIORITY_PURIFY( priority );

  if ( priority != _Scheduler_SMP_Node_priority( node ) ) {
    ( *update )( context, node, priority );
  }

  if ( node_state == SCHEDULER_SMP_NODE_BLOCKED ) {
    Priority_Control insert_priority;
    bool             needs_help;

    insert_priority = SCHEDULER_PRIORITY_APPEND( priority );
    needs_help = ( *enqueue )( context, node, insert_priority );

    if ( needs_help && thread->Scheduler.helping_nodes > 0 ) {
      _Scheduler_SMP_Request_ask_for_help( thread );
    }
  } else {
    _Assert( node_state == SCHEDULER_SMP_NODE_READY );
    _Assert( node->sticky_level > 0 );
    _Assert( node->idle == NULL );
    _Scheduler_SMP_Request_ask_for_help( thread );
  }
}

/**
 * @brief Updates the priority of the node and the position in the queues it
 * is in.
 *
 * This function firstly updates the priority of the node and then extracts
 * and reinserts it into the queue the node is part of using the given
 * functions.
 *
 * @param context The scheduler instance context.
 * @param thread The thread for the operation.
 * @param[in, out] node The node to update the priority of.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the ready
 *      queue of the scheduler context.
 * @param update Function to update the priority of a node in the scheduler
 *      context.
 * @param enqueue Function to enqueue a node with a given priority.
 * @param enqueue_scheduled Function to enqueue a scheduled node.
 * @param ask_for_help Function to perform a help request.
 */
static inline void _Scheduler_SMP_Update_priority(
  Scheduler_Context              *context,
  Thread_Control                 *thread,
  Scheduler_Node                 *node,
  Scheduler_SMP_Extract           extract_from_scheduled,
  Scheduler_SMP_Extract           extract_from_ready,
  Scheduler_SMP_Update            update,
  Scheduler_SMP_Enqueue           enqueue,
  Scheduler_SMP_Enqueue_scheduled enqueue_scheduled,
  Scheduler_SMP_Ask_for_help      ask_for_help
)
{
  Priority_Control         priority;
  Priority_Control         insert_priority;
  Scheduler_SMP_Node_state node_state;

  insert_priority = _Scheduler_Node_get_priority( node );
  priority = SCHEDULER_PRIORITY_PURIFY( insert_priority );

  if ( priority == _Scheduler_SMP_Node_priority( node ) ) {
    if ( _Thread_Is_ready( thread ) ) {
      ( *ask_for_help )( context, thread, node );
    }

    return;
  }

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    ( *extract_from_scheduled )( context, node );
    ( *update )( context, node, priority );
    ( *enqueue_scheduled )( context, node, insert_priority );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    ( *extract_from_ready )( context, node );
    ( *update )( context, node, priority );
    ( *enqueue )( context, node, insert_priority );
  } else {
    ( *update )( context, node, priority );

    if ( _Thread_Is_ready( thread ) ) {
      ( *ask_for_help )( context, thread, node );
    }
  }
}

/**
 * @brief Performs a yield and asks for help if necessary.
 *
 * @param context The scheduler instance context.
 * @param thread The thread for the operation.
 * @param node The node of the thread that yields.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the ready
 *      queue of the scheduler context.
 * @param enqueue Function to enqueue a node with a given priority.
 * @param enqueue_scheduled Function to enqueue a scheduled node.
 */
static inline void _Scheduler_SMP_Yield(
  Scheduler_Context              *context,
  Thread_Control                 *thread,
  Scheduler_Node                 *node,
  Scheduler_SMP_Extract           extract_from_scheduled,
  Scheduler_SMP_Extract           extract_from_ready,
  Scheduler_SMP_Enqueue           enqueue,
  Scheduler_SMP_Enqueue_scheduled enqueue_scheduled
)
{
  Scheduler_SMP_Node_state node_state;
  Priority_Control         insert_priority;

  node_state = _Scheduler_SMP_Node_state( node );
  insert_priority = _Scheduler_SMP_Node_priority( node );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    ( *extract_from_scheduled )( context, node );
    ( *enqueue_scheduled )( context, node, insert_priority );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    ( *extract_from_ready )( context, node );
    (void) ( *enqueue )( context, node, insert_priority );
  }
}

/**
 * @brief Inserts the node with the given priority into the scheduled nodes.
 *
 * @param context The scheduler instance context.
 * @param node_to_insert The scheduled node to insert.
 * @param priority_to_insert The priority with which to insert the node.
 */
static inline void _Scheduler_SMP_Insert_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert,
  Priority_Control   priority_to_insert
)
{
  Scheduler_SMP_Context *self;

  self = _Scheduler_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &node_to_insert->Node.Chain,
    &priority_to_insert,
    _Scheduler_SMP_Priority_less_equal
  );
}

/**
 * @brief Asks for help.
 *
 * @param context The scheduler instance context.
 * @param thread The thread that asks for help.
 * @param[in, out] node The node of the thread that performs the ask for help
 *      operation.
 * @param order The order function.
 * @param insert_ready Function to insert a node into the set of ready
 *      nodes.
 * @param insert_scheduled Function to insert a node into the set of
 *      scheduled nodes.
 * @param move_from_scheduled_to_ready Function to move a node from the set
 *      of scheduled nodes to the set of ready nodes.
 * @param get_lowest_scheduled Function to select the node from the
 *      scheduled nodes to replace.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 *
 * @retval true The ask for help operation was successful.
 * @retval false The ask for help operation was not successful.
 */
static inline bool _Scheduler_SMP_Ask_for_help(
  Scheduler_Context                  *context,
  Thread_Control                     *thread,
  Scheduler_Node                     *node,
  Chain_Node_order                    order,
  Scheduler_SMP_Insert                insert_ready,
  Scheduler_SMP_Insert                insert_scheduled,
  Scheduler_SMP_Move                  move_from_scheduled_to_ready,
  Scheduler_SMP_Get_lowest_scheduled  get_lowest_scheduled,
  Scheduler_SMP_Allocate_processor    allocate_processor,
  Scheduler_Release_idle_node         release_idle_node
)
{
  Scheduler_Node   *lowest_scheduled;
  ISR_lock_Context  lock_context;
  bool              success;

  if ( thread->Scheduler.pinned_scheduler != NULL ) {
    /*
     * Pinned threads are not allowed to ask for help.  Return success to break
     * the loop in _Thread_Ask_for_help() early.
     */
    return true;
  }

  lowest_scheduled = ( *get_lowest_scheduled )( context, node );

  _Thread_Scheduler_acquire_critical( thread, &lock_context );

  if ( thread->Scheduler.state == THREAD_SCHEDULER_READY ) {
    Scheduler_SMP_Node_state node_state;

    node_state = _Scheduler_SMP_Node_state( node );

    if ( node_state == SCHEDULER_SMP_NODE_BLOCKED ) {
      Priority_Control insert_priority;

      insert_priority = _Scheduler_SMP_Node_priority( node );

      if (
        ( *order )(
          &insert_priority,
          &node->Node.Chain,
          &lowest_scheduled->Node.Chain
        )
      ) {
        Thread_Control *lowest_scheduled_idle;

        _Scheduler_SMP_Cancel_ask_for_help( thread );
        _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_SCHEDULED );
        _Thread_Scheduler_release_critical( thread, &lock_context );

        lowest_scheduled_idle = _Scheduler_Release_idle_thread_if_necessary(
          lowest_scheduled,
          release_idle_node,
          context
        );

        _Scheduler_SMP_Preempt(
          context,
          node,
          lowest_scheduled,
          lowest_scheduled_idle,
          allocate_processor
        );

        ( *move_from_scheduled_to_ready )( context, lowest_scheduled );
        ( *insert_scheduled )( context, node, insert_priority );

        success = true;
      } else {
        _Thread_Scheduler_release_critical( thread, &lock_context );

        _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
        ( *insert_ready )( context, node, insert_priority );
        success = false;
      }
    } else if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
      _Scheduler_SMP_Cancel_ask_for_help( thread );
      _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_SCHEDULED );
      _Thread_Scheduler_release_critical( thread, &lock_context );
      _Scheduler_Discard_idle_thread(
        thread,
        node,
        release_idle_node,
        context
      );
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

/**
 * @brief Reconsiders help request.
 *
 * @param context The scheduler context instance.
 * @param thread The thread to reconsider the help request of.
 * @param[in, out] node The scheduler node of @a thread.
 * @param extract_from_ready Function to extract a node from the ready queue
 *      of the scheduler context.
 */
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

/**
 * @brief Withdraws the node.
 *
 * @param context The scheduler context instance.
 * @param[in, out] thread The thread to change to @a next_state.
 * @param[in, out] node The node to withdraw.
 * @param next_state The new state for @a thread.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the ready queue
 *      of the scheduler context.
 * @param get_highest_ready Function to get the highest ready node.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *      of ready nodes to the set of scheduled nodes.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Withdraw_node(
  Scheduler_Context                *context,
  Thread_Control                   *thread,
  Scheduler_Node                   *node,
  Thread_Scheduler_state            next_state,
  Scheduler_SMP_Extract             extract_from_scheduled,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node
)
{
  ISR_lock_Context         lock_context;
  Scheduler_SMP_Node_state node_state;

  _Thread_Scheduler_acquire_critical( thread, &lock_context );

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    Per_CPU_Control *cpu;

    _Assert( thread == _Scheduler_Node_get_user( node ) );
    cpu = _Thread_Get_CPU( thread );
    _Scheduler_Thread_change_state( thread, next_state );
    _Thread_Scheduler_release_critical( thread, &lock_context );

    _Assert( _Scheduler_Node_get_user( node ) == thread );
    _Assert( _Scheduler_Node_get_idle( node ) == NULL );

    _Scheduler_SMP_Schedule_highest_ready(
      context,
      node,
      cpu,
      extract_from_scheduled,
      extract_from_ready,
      get_highest_ready,
      move_from_ready_to_scheduled,
      allocate_processor,
      get_idle_node
    );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    _Thread_Scheduler_release_critical( thread, &lock_context );
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );
    ( *extract_from_ready )( context, node );
  } else {
    _Assert( node_state == SCHEDULER_SMP_NODE_BLOCKED );
    _Thread_Scheduler_release_critical( thread, &lock_context );
  }
}

/**
 * @brief Makes the node sticky.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to make sticky.
 */
static inline void _Scheduler_SMP_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Scheduler_SMP_Update     update,
  Scheduler_SMP_Enqueue    enqueue
)
{
  Scheduler_SMP_Node_state node_state;

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_BLOCKED ) {
    Scheduler_Context *context;
    Priority_Control   insert_priority;
    Priority_Control   priority;

    context = _Scheduler_Get_context( scheduler );
    priority = _Scheduler_Node_get_priority( node );
    priority = SCHEDULER_PRIORITY_PURIFY( priority );

    if ( priority != _Scheduler_SMP_Node_priority( node ) ) {
      ( *update )( context, node, priority );
    }

    insert_priority = SCHEDULER_PRIORITY_APPEND( priority );
    (void) ( *enqueue )( context, node, insert_priority );
  }
}

/**
 * @brief Cleans the sticky property from the node.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] the_thread is the thread owning the node.
 *
 * @param[in, out] node is the scheduler node to clean the sticky property.
 */
static inline void _Scheduler_SMP_Clean_sticky(
  const Scheduler_Control          *scheduler,
  Thread_Control                   *the_thread,
  Scheduler_Node                   *node,
  Scheduler_SMP_Extract             extract_from_scheduled,
  Scheduler_SMP_Extract             extract_from_ready,
  Scheduler_SMP_Get_highest_ready   get_highest_ready,
  Scheduler_SMP_Move                move_from_ready_to_scheduled,
  Scheduler_SMP_Allocate_processor  allocate_processor,
  Scheduler_Get_idle_node           get_idle_node,
  Scheduler_Release_idle_node       release_idle_node
)
{
  Scheduler_SMP_Node_state node_state;

  node_state = _Scheduler_SMP_Node_state( node );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    Thread_Control *idle;

    idle = _Scheduler_Node_get_idle( node );

    if ( idle != NULL ) {
      Scheduler_Context *context;

      context = _Scheduler_Get_context( scheduler );

      _Scheduler_Release_idle_thread( node, idle, release_idle_node, context );
      _Scheduler_SMP_Schedule_highest_ready(
        context,
        node,
        _Thread_Get_CPU( idle ),
        extract_from_scheduled,
        extract_from_ready,
        get_highest_ready,
        move_from_ready_to_scheduled,
        allocate_processor,
        get_idle_node
      );
    }
  }
}

/**
 * @brief Starts the idle thread on the given processor.
 *
 * @param context The scheduler context instance.
 * @param[in, out] idle The idle thread to schedule.
 * @param cpu The processor for the idle thread.
 * @param register_idle Function to register the idle thread for a cpu.
 */
static inline void _Scheduler_SMP_Do_start_idle(
  Scheduler_Context           *context,
  Thread_Control              *idle,
  Per_CPU_Control             *cpu,
  Scheduler_SMP_Register_idle  register_idle
)
{
  Scheduler_SMP_Context *self;
  Scheduler_SMP_Node    *node;

  self = _Scheduler_SMP_Get_self( context );
  node = _Scheduler_SMP_Thread_get_node( idle );

  _Scheduler_Thread_change_state( idle, THREAD_SCHEDULER_SCHEDULED );
  node->state = SCHEDULER_SMP_NODE_SCHEDULED;

  _Thread_Set_CPU( idle, cpu );
  ( *register_idle )( context, &node->Base, cpu );
  _Chain_Append_unprotected( &self->Scheduled, &node->Base.Node.Chain );
}

/**
 * @brief Adds the idle thread to the processor.
 *
 * @param context The scheduler context instance.
 * @param[in, out] idle The idle thread to add to the processor.
 * @param has_ready Function that checks if a given context has ready threads.
 * @param enqueue_scheduled Function to enqueue a scheduled node.
 * @param register_idle Function to register the idle thread for a cpu.
 */
static inline void _Scheduler_SMP_Add_processor(
  Scheduler_Context              *context,
  Thread_Control                 *idle,
  Scheduler_SMP_Has_ready         has_ready,
  Scheduler_SMP_Enqueue_scheduled enqueue_scheduled,
  Scheduler_SMP_Register_idle     register_idle
)
{
  Scheduler_SMP_Context *self;
  Scheduler_Node        *node;

  self = _Scheduler_SMP_Get_self( context );
  idle->Scheduler.state = THREAD_SCHEDULER_SCHEDULED;
  node = _Thread_Scheduler_get_home_node( idle );
  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_SCHEDULED );
  ( *register_idle )( context, node, _Thread_Get_CPU( idle ) );

  if ( ( *has_ready )( &self->Base ) ) {
    Priority_Control insert_priority;

    insert_priority = _Scheduler_SMP_Node_priority( node );
    insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
    ( *enqueue_scheduled )( &self->Base, node, insert_priority );
  } else {
    _Chain_Append_unprotected( &self->Scheduled, &node->Node.Chain );
  }
}

/**
 * @brief Removes an idle thread from the processor.
 *
 * @param context The scheduler context instance.
 * @param cpu The processor to remove from.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the ready queue
 *      of the scheduler context.
 * @param enqueue Function to enqueue a node with a given priority.
 *
 * @return The idle thread of @a cpu.
 */
static inline Thread_Control *_Scheduler_SMP_Remove_processor(
  Scheduler_Context          *context,
  Per_CPU_Control            *cpu,
  Scheduler_SMP_Extract       extract_from_scheduled,
  Scheduler_SMP_Extract       extract_from_ready,
  Scheduler_SMP_Enqueue       enqueue,
  Scheduler_Get_idle_node     get_idle_node,
  Scheduler_Release_idle_node release_idle_node
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

  ( *extract_from_scheduled )( &self->Base, victim_node );
  victim_owner = _Scheduler_Node_get_owner( victim_node );

  if ( !victim_owner->is_idle ) {
    Thread_Control  *victim_idle;
    Scheduler_Node  *idle_node;
    Priority_Control insert_priority;

    victim_idle = _Scheduler_Release_idle_thread_if_necessary(
      victim_node,
      release_idle_node,
      &self->Base
    );
    idle_node = ( *get_idle_node )( &self->Base );
    idle = _Scheduler_Node_get_owner( idle_node );
    _Scheduler_SMP_Preempt(
      &self->Base,
      idle_node,
      victim_node,
      victim_idle,
      _Scheduler_SMP_Allocate_processor_exact
    );

    _Assert( !_Chain_Is_empty( &self->Scheduled ) );
    insert_priority = _Scheduler_SMP_Node_priority( victim_node );
    insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
    ( *enqueue )( &self->Base, victim_node, insert_priority );
  } else {
    _Assert( victim_owner == victim_user );
    _Assert( _Scheduler_Node_get_idle( victim_node ) == NULL );
    idle = victim_owner;
  }

  return idle;
}

/**
 * @brief Sets the affinity of the node.
 *
 * Also performs a reinsert into the queue the node is currently in.
 *
 * @param context The scheduler context instance.
 * @param thread The thread for the operation.
 * @param[in, out] node The node to set the affinity of.
 * @param arg The affinity for @a node.
 * @param set_affinity Function to set the affinity of a node.
 * @param extract_from_scheduled Function to extract a node from the set of
 *      scheduled nodes.
 * @param extract_from_ready Function to extract a node from the ready queue
 *      of the scheduler context.
 * @param get_highest_ready Function to get the highest ready node.
 * @param move_from_ready_to_scheduled Function to move a node from the set
 *      of ready nodes to the set of scheduled nodes.
 * @param enqueue Function to enqueue a node with a given priority.
 * @param allocate_processor Function to allocate a processor to a node
 *      based on the rules of the scheduler.
 */
static inline void _Scheduler_SMP_Set_affinity(
  Scheduler_Context               *context,
  Thread_Control                  *thread,
  Scheduler_Node                  *node,
  void                            *arg,
  Scheduler_SMP_Set_affinity       set_affinity,
  Scheduler_SMP_Extract            extract_from_scheduled,
  Scheduler_SMP_Extract            extract_from_ready,
  Scheduler_SMP_Get_highest_ready  get_highest_ready,
  Scheduler_SMP_Move               move_from_ready_to_scheduled,
  Scheduler_SMP_Enqueue            enqueue,
  Scheduler_SMP_Allocate_processor allocate_processor,
  Scheduler_Get_idle_node          get_idle_node,
  Scheduler_Release_idle_node      release_idle_node
)
{
  Scheduler_SMP_Node_state node_state;
  Priority_Control         insert_priority;

  node_state = _Scheduler_SMP_Node_state( node );
  insert_priority = _Scheduler_SMP_Node_priority( node );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );

  if ( node_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    ( *extract_from_scheduled )( context, node );
    _Scheduler_SMP_Preempt_and_schedule_highest_ready(
      context,
      node,
      extract_from_ready,
      get_highest_ready,
      move_from_ready_to_scheduled,
      allocate_processor,
      get_idle_node,
      release_idle_node
    );
    ( *set_affinity )( context, node, arg );
    ( *enqueue )( context, node, insert_priority );
  } else if ( node_state == SCHEDULER_SMP_NODE_READY ) {
    ( *extract_from_ready )( context, node );
    ( *set_affinity )( context, node, arg );
    ( *enqueue )( context, node, insert_priority );
  } else {
    _Assert( node_state == SCHEDULER_SMP_NODE_BLOCKED );
    ( *set_affinity )( context, node, arg );
  }
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
