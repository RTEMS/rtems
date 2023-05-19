/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSMPEDF
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_EDF_SMP_Add_processor(), _Scheduler_EDF_SMP_Ask_for_help(),
 *   _Scheduler_EDF_SMP_Block(), _Scheduler_EDF_SMP_Initialize(),
 *   _Scheduler_EDF_SMP_Node_initialize(), _Scheduler_EDF_SMP_Pin(),
 *   _Scheduler_EDF_SMP_Reconsider_help_request(),
 *   _Scheduler_EDF_SMP_Remove_processor(), _Scheduler_EDF_SMP_Set_affinity(),
 *   _Scheduler_EDF_SMP_Start_idle(), _Scheduler_EDF_SMP_Unblock(),
 *   _Scheduler_EDF_SMP_Unpin(), _Scheduler_EDF_SMP_Update_priority(),
 *   _Scheduler_EDF_SMP_Withdraw_node(), _Scheduler_EDF_SMP_Make_sticky(),
 *   _Scheduler_EDF_SMP_Clean_sticky(), and _Scheduler_EDF_SMP_Yield().
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/scheduleredfsmp.h>
#include <rtems/score/schedulersmpimpl.h>

static inline Scheduler_EDF_SMP_Context *
_Scheduler_EDF_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_EDF_SMP_Context *) _Scheduler_Get_context( scheduler );
}

static inline Scheduler_EDF_SMP_Context *
_Scheduler_EDF_SMP_Get_self( Scheduler_Context *context )
{
  return (Scheduler_EDF_SMP_Context *) context;
}

static inline Scheduler_EDF_SMP_Node *
_Scheduler_EDF_SMP_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_EDF_SMP_Node *) node;
}

static inline bool _Scheduler_EDF_SMP_Priority_less_equal(
  const void        *left,
  const RBTree_Node *right
)
{
  const Priority_Control   *the_left;
  const Scheduler_SMP_Node *the_right;
  Priority_Control          prio_left;
  Priority_Control          prio_right;

  the_left = left;
  the_right = RTEMS_CONTAINER_OF( right, Scheduler_SMP_Node, Base.Node.RBTree );

  prio_left = *the_left;
  prio_right = the_right->priority;

  return prio_left <= prio_right;
}

static inline bool _Scheduler_EDF_SMP_Overall_less_equal(
  const void       *key,
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  Priority_Control              insert_priority;
  Priority_Control              next_priority;
  const Scheduler_EDF_SMP_Node *node_to_insert;
  const Scheduler_EDF_SMP_Node *node_next;

  insert_priority = *(const Priority_Control *) key;
  insert_priority = SCHEDULER_PRIORITY_PURIFY( insert_priority );
  node_to_insert = (const Scheduler_EDF_SMP_Node *) to_insert;
  node_next = (const Scheduler_EDF_SMP_Node *) next;
  next_priority = node_next->Base.priority;

  return insert_priority < next_priority ||
    ( insert_priority == next_priority &&
      node_to_insert->generation <= node_next->generation );
}

void _Scheduler_EDF_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->Affine_queues );
  /* The ready queues are zero initialized and thus empty */
}

void _Scheduler_EDF_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_SMP_Node *smp_node;

  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize( scheduler, smp_node, the_thread, priority );
}

static inline void _Scheduler_EDF_SMP_Do_update(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   new_priority
)
{
  Scheduler_SMP_Node *smp_node;

  (void) context;

  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_update_priority( smp_node, new_priority );
}

static inline bool _Scheduler_EDF_SMP_Has_ready( Scheduler_Context *context )
{
  Scheduler_EDF_SMP_Context *self = _Scheduler_EDF_SMP_Get_self( context );

  return !_RBTree_Is_empty( &self->Ready[ 0 ].Queue );
}

static inline bool _Scheduler_EDF_SMP_Overall_less(
  const Scheduler_EDF_SMP_Node *left,
  const Scheduler_EDF_SMP_Node *right
)
{
  Priority_Control lp;
  Priority_Control rp;

  lp = left->Base.priority;
  rp = right->Base.priority;

  return lp < rp || (lp == rp && left->generation < right->generation );
}

static inline Scheduler_EDF_SMP_Node *
_Scheduler_EDF_SMP_Challenge_highest_ready(
  Scheduler_EDF_SMP_Context *self,
  Scheduler_EDF_SMP_Node    *highest_ready,
  RBTree_Control            *ready_queue
)
{
  Scheduler_EDF_SMP_Node *other;

  other = (Scheduler_EDF_SMP_Node *) _RBTree_Minimum( ready_queue );
  _Assert( other != NULL );

  if ( _Scheduler_EDF_SMP_Overall_less( other, highest_ready ) ) {
    return other;
  }

  return highest_ready;
}

static inline Scheduler_Node *_Scheduler_EDF_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
)
{
  Scheduler_EDF_SMP_Context *self;
  Scheduler_EDF_SMP_Node    *highest_ready;
  Scheduler_EDF_SMP_Node    *node;
  uint8_t                    rqi;
  const Chain_Node          *tail;
  Chain_Node                *next;

  self = _Scheduler_EDF_SMP_Get_self( context );
  highest_ready = (Scheduler_EDF_SMP_Node *)
    _RBTree_Minimum( &self->Ready[ 0 ].Queue );
  _Assert( highest_ready != NULL );

  /*
   * The filter node is a scheduled node which is no longer on the scheduled
   * chain.  In case this is an affine thread, then we have to check the
   * corresponding affine ready queue.
   */

  node = (Scheduler_EDF_SMP_Node *) filter;
  rqi = node->ready_queue_index;

  if ( rqi != 0 && !_RBTree_Is_empty( &self->Ready[ rqi ].Queue ) ) {
    highest_ready = _Scheduler_EDF_SMP_Challenge_highest_ready(
      self,
      highest_ready,
      &self->Ready[ rqi ].Queue
    );
  }

  tail = _Chain_Immutable_tail( &self->Affine_queues );
  next = _Chain_First( &self->Affine_queues );

  while ( next != tail ) {
    Scheduler_EDF_SMP_Ready_queue *ready_queue;

    ready_queue = (Scheduler_EDF_SMP_Ready_queue *) next;
    highest_ready = _Scheduler_EDF_SMP_Challenge_highest_ready(
      self,
      highest_ready,
      &ready_queue->Queue
    );

    next = _Chain_Next( next );
  }

  return &highest_ready->Base.Base;
}

static inline void _Scheduler_EDF_SMP_Set_allocated(
  Scheduler_EDF_SMP_Context *self,
  Scheduler_EDF_SMP_Node    *allocated,
  const Per_CPU_Control     *cpu
)
{
  self->Ready[ _Per_CPU_Get_index( cpu ) + 1 ].allocated = allocated;
}

static inline Scheduler_EDF_SMP_Node *_Scheduler_EDF_SMP_Get_allocated(
  const Scheduler_EDF_SMP_Context *self,
  uint8_t                          rqi
)
{
  return self->Ready[ rqi ].allocated;
}

static inline Scheduler_Node *_Scheduler_EDF_SMP_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{
  Scheduler_EDF_SMP_Node *filter;
  uint8_t                 rqi;

  filter = _Scheduler_EDF_SMP_Node_downcast( filter_base );
  rqi = filter->ready_queue_index;

  if ( rqi != 0 ) {
    Scheduler_EDF_SMP_Context *self;
    Scheduler_EDF_SMP_Node    *affine_scheduled;

    self = _Scheduler_EDF_SMP_Get_self( context );
    affine_scheduled = self->Ready[ rqi ].affine_scheduled;

    if ( affine_scheduled != NULL ) {
      _Assert( affine_scheduled->ready_queue_index == rqi );
      return &affine_scheduled->Base.Base;
    }
  }

  return _Scheduler_SMP_Get_lowest_scheduled( context, filter_base );
}

static inline void _Scheduler_EDF_SMP_Update_generation(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_EDF_SMP_Context *self;
  Scheduler_EDF_SMP_Node    *node;
  int                        generation_index;
  int                        increment;
  int64_t                    generation;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  generation_index = SCHEDULER_PRIORITY_IS_APPEND( insert_priority );
  increment = ( generation_index << 1 ) - 1;

  generation = self->generations[ generation_index ];
  node->generation = generation;
  self->generations[ generation_index ] = generation + increment;
}

static inline void _Scheduler_EDF_SMP_Insert_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   priority_to_insert
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *node;
  uint8_t                        rqi;
  Scheduler_EDF_SMP_Ready_queue *ready_queue;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  rqi = node->ready_queue_index;
  ready_queue = &self->Ready[ rqi ];

  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &node_base->Node.Chain,
    &priority_to_insert,
    _Scheduler_EDF_SMP_Overall_less_equal
  );

  if ( rqi != 0 ) {
    ready_queue->affine_scheduled = node;

    if ( !_RBTree_Is_empty( &ready_queue->Queue ) ) {
      _Chain_Extract_unprotected( &ready_queue->Node );
    }
  }
}

static inline void _Scheduler_EDF_SMP_Activate_ready_queue_if_necessary(
  Scheduler_EDF_SMP_Context     *self,
  uint8_t                        rqi,
  Scheduler_EDF_SMP_Ready_queue *ready_queue
)
{
  if (
    rqi != 0 &&
    _RBTree_Is_empty( &ready_queue->Queue ) &&
    ready_queue->affine_scheduled == NULL
  ) {
    _Chain_Append_unprotected( &self->Affine_queues, &ready_queue->Node );
  }
}

static inline void _Scheduler_EDF_SMP_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *node;
  uint8_t                        rqi;
  Scheduler_EDF_SMP_Ready_queue *ready_queue;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  rqi = node->ready_queue_index;
  ready_queue = &self->Ready[ rqi ];

  _Scheduler_EDF_SMP_Activate_ready_queue_if_necessary( self, rqi, ready_queue );
  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Insert_inline(
    &ready_queue->Queue,
    &node->Base.Base.Node.RBTree,
    &insert_priority,
    _Scheduler_EDF_SMP_Priority_less_equal
  );
}

static inline void _Scheduler_EDF_SMP_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *node;
  uint8_t                        rqi;
  Scheduler_EDF_SMP_Ready_queue *ready_queue;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_to_extract );

  _Scheduler_SMP_Extract_from_scheduled( &self->Base.Base, &node->Base.Base );

  rqi = node->ready_queue_index;
  ready_queue = &self->Ready[ rqi ];

  if ( rqi != 0 && !_RBTree_Is_empty( &ready_queue->Queue ) ) {
    _Chain_Append_unprotected( &self->Affine_queues, &ready_queue->Node );
  }

  ready_queue->affine_scheduled = NULL;
}

static inline void _Scheduler_EDF_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *node;
  uint8_t                        rqi;
  Scheduler_EDF_SMP_Ready_queue *ready_queue;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_to_extract );
  rqi = node->ready_queue_index;
  ready_queue = &self->Ready[ rqi ];

  _RBTree_Extract( &ready_queue->Queue, &node->Base.Base.Node.RBTree );
  _Chain_Initialize_node( &node->Base.Base.Node.Chain );

  if (
    rqi != 0
      && _RBTree_Is_empty( &ready_queue->Queue )
      && ready_queue->affine_scheduled == NULL
  ) {
    _Chain_Extract_unprotected( &ready_queue->Node );
  }
}

static inline void _Scheduler_EDF_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *node;
  uint8_t                        rqi;
  Scheduler_EDF_SMP_Ready_queue *ready_queue;

  _Scheduler_EDF_SMP_Extract_from_scheduled( context, scheduled_to_ready );

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( scheduled_to_ready );
  rqi = node->ready_queue_index;
  ready_queue = &self->Ready[ rqi ];

  _Scheduler_EDF_SMP_Activate_ready_queue_if_necessary( self, rqi, ready_queue );
  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Prepend( &ready_queue->Queue, &node->Base.Base.Node.RBTree );
}

static inline void _Scheduler_EDF_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Priority_Control insert_priority;

  _Scheduler_EDF_SMP_Extract_from_ready( context, ready_to_scheduled );
  insert_priority = _Scheduler_SMP_Node_priority( ready_to_scheduled );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Scheduler_EDF_SMP_Insert_scheduled(
    context,
    ready_to_scheduled,
    insert_priority
  );
}

static inline Scheduler_Node *_Scheduler_EDF_SMP_Get_idle( void *arg )
{
  Scheduler_EDF_SMP_Context *self;
  Scheduler_Node            *lowest_ready;

  self = _Scheduler_EDF_SMP_Get_self( arg );
  lowest_ready = (Scheduler_Node *) _RBTree_Maximum( &self->Ready[ 0 ].Queue );
  _Assert( lowest_ready != NULL );
  _RBTree_Extract( &self->Ready[ 0 ].Queue, &lowest_ready->Node.RBTree );
  _Chain_Initialize_node( &lowest_ready->Node.Chain );

  return lowest_ready;
}

static inline void _Scheduler_EDF_SMP_Release_idle(
  Scheduler_Node *node,
  void           *arg
)
{
  Scheduler_EDF_SMP_Context *self;

  self = _Scheduler_EDF_SMP_Get_self( arg );
  _RBTree_Initialize_node( &node->Node.RBTree );
  _RBTree_Append( &self->Ready[ 0 ].Queue, &node->Node.RBTree );
}

static inline void _Scheduler_EDF_SMP_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Per_CPU_Control   *cpu
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *scheduled;
  uint8_t                        rqi;

  self = _Scheduler_EDF_SMP_Get_self( context );
  scheduled = _Scheduler_EDF_SMP_Node_downcast( scheduled_base );
  rqi = scheduled->ready_queue_index;

  if ( rqi != 0 ) {
    Per_CPU_Control *affine_cpu;

    affine_cpu = _Per_CPU_Get_by_index( rqi - 1 );

    if ( cpu != affine_cpu ) {
      Scheduler_EDF_SMP_Node *node;

      node = _Scheduler_EDF_SMP_Get_allocated( self, rqi );
      _Assert( node->ready_queue_index == 0 );
      _Scheduler_EDF_SMP_Set_allocated( self, node, cpu );
      _Scheduler_SMP_Allocate_processor_exact(
        context,
        &node->Base.Base,
        cpu
      );
      cpu = affine_cpu;
    }
  }

  _Scheduler_EDF_SMP_Set_allocated( self, scheduled, cpu );
  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &scheduled->Base.Base,
    cpu
  );
}

void _Scheduler_EDF_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Get_idle
  );
}

static inline bool _Scheduler_EDF_SMP_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  _Scheduler_EDF_SMP_Update_generation( context, node, insert_priority );

  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_EDF_SMP_Overall_less_equal,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_EDF_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Get_lowest_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Get_idle,
    _Scheduler_EDF_SMP_Release_idle
  );
}

static inline void _Scheduler_EDF_SMP_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  _Scheduler_EDF_SMP_Update_generation( context, node, insert_priority );
  _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_EDF_SMP_Overall_less_equal,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_EDF_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Get_idle,
    _Scheduler_EDF_SMP_Release_idle
  );
}

void _Scheduler_EDF_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_EDF_SMP_Do_update,
    _Scheduler_EDF_SMP_Enqueue,
    _Scheduler_EDF_SMP_Release_idle
  );
}

static inline bool _Scheduler_EDF_SMP_Do_ask_for_help(
  Scheduler_Context *context,
  Thread_Control    *the_thread,
  Scheduler_Node    *node
)
{
  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_EDF_SMP_Overall_less_equal,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_EDF_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_EDF_SMP_Get_lowest_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Release_idle
  );
}

void _Scheduler_EDF_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Do_update,
    _Scheduler_EDF_SMP_Enqueue,
    _Scheduler_EDF_SMP_Enqueue_scheduled,
    _Scheduler_EDF_SMP_Do_ask_for_help
  );
}

bool _Scheduler_EDF_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_EDF_SMP_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_EDF_SMP_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Reconsider_help_request(
    context,
    the_thread,
    node,
    _Scheduler_EDF_SMP_Extract_from_ready
  );
}

void _Scheduler_EDF_SMP_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Get_idle
  );
}

void _Scheduler_EDF_SMP_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Make_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_EDF_SMP_Do_update,
    _Scheduler_EDF_SMP_Enqueue
  );
}

void _Scheduler_EDF_SMP_Clean_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Clean_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor,
    _Scheduler_EDF_SMP_Get_idle,
    _Scheduler_EDF_SMP_Release_idle
  );
}

static inline void _Scheduler_EDF_SMP_Register_idle(
  Scheduler_Context *context,
  Scheduler_Node    *idle_base,
  Per_CPU_Control   *cpu
)
{
  Scheduler_EDF_SMP_Context *self;
  Scheduler_EDF_SMP_Node    *idle;

  self = _Scheduler_EDF_SMP_Get_self( context );
  idle = _Scheduler_EDF_SMP_Node_downcast( idle_base );
  _Scheduler_EDF_SMP_Set_allocated( self, idle, cpu );
  _Scheduler_EDF_SMP_Update_generation(
    context,
    idle_base,
    PRIORITY_GROUP_LAST
  );
}

void _Scheduler_EDF_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_EDF_SMP_Has_ready,
    _Scheduler_EDF_SMP_Enqueue_scheduled,
    _Scheduler_EDF_SMP_Register_idle
  );
}

Thread_Control *_Scheduler_EDF_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Enqueue,
    _Scheduler_EDF_SMP_Get_idle,
    _Scheduler_EDF_SMP_Release_idle
  );
}

void _Scheduler_EDF_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    thread,
    node,
    _Scheduler_EDF_SMP_Extract_from_scheduled,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Enqueue,
    _Scheduler_EDF_SMP_Enqueue_scheduled
  );
}

static inline void _Scheduler_EDF_SMP_Do_set_affinity(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  void              *arg
)
{
  Scheduler_EDF_SMP_Node *node;
  const uint8_t          *rqi;

  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  rqi = arg;
  node->ready_queue_index = *rqi;
}

void _Scheduler_EDF_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context;

  context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Do_start_idle(
    context,
    idle,
    cpu,
    _Scheduler_EDF_SMP_Register_idle
  );
}

void _Scheduler_EDF_SMP_Pin(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  struct Per_CPU_Control  *cpu
)
{
  Scheduler_EDF_SMP_Node *node;
  uint8_t                 rqi;

  (void) scheduler;
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );

  _Assert(
    _Scheduler_SMP_Node_state( &node->Base.Base ) == SCHEDULER_SMP_NODE_BLOCKED
  );

  rqi = (uint8_t) _Per_CPU_Get_index( cpu ) + 1;
  node->ready_queue_index = rqi;
  node->pinning_ready_queue_index = rqi;
}

void _Scheduler_EDF_SMP_Unpin(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  struct Per_CPU_Control  *cpu
)
{
  Scheduler_EDF_SMP_Node *node;

  (void) scheduler;
  (void) cpu;
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );

  _Assert(
    _Scheduler_SMP_Node_state( &node->Base.Base ) == SCHEDULER_SMP_NODE_BLOCKED
  );

  node->ready_queue_index = node->affinity_ready_queue_index;
  node->pinning_ready_queue_index = 0;
}

Status_Control _Scheduler_EDF_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context      *context;
  Scheduler_EDF_SMP_Node *node;
  uint8_t                 rqi;

  context = _Scheduler_Get_context( scheduler );

  /*
   * We support a thread to processor affinity to all online processors and an
   * affinity to exactly one processor.  This restriction is necessary to avoid
   * issues if processors are added or removed to or from the scheduler.
   */

  if ( _Processor_mask_Is_equal( affinity, &_SMP_Online_processors ) ) {
    rqi = 0;
  } else {
    Processor_mask local_affinity;
    Processor_mask one_to_one;
    uint32_t       last;

    _Processor_mask_And( &local_affinity, &context->Processors, affinity );

    if ( _Processor_mask_Is_zero( &local_affinity ) ) {
      return STATUS_INVALID_NUMBER;
    }

    last = _Processor_mask_Find_last_set( affinity );
    _Processor_mask_From_index( &one_to_one, last - 1 );

    /*
     * Use the global affinity set and not the affinity set local to the
     * scheduler to check for a one-to-one affinity.
     */
    if ( !_Processor_mask_Is_equal( &one_to_one, affinity ) ) {
      return STATUS_INVALID_NUMBER;
    }

    rqi = last;
  }

  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  node->affinity_ready_queue_index = rqi;

  if ( node->pinning_ready_queue_index == 0 ) {
    _Scheduler_SMP_Set_affinity(
      context,
      thread,
      node_base,
      &rqi,
      _Scheduler_EDF_SMP_Do_set_affinity,
      _Scheduler_EDF_SMP_Extract_from_scheduled,
      _Scheduler_EDF_SMP_Extract_from_ready,
      _Scheduler_EDF_SMP_Get_highest_ready,
      _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
      _Scheduler_EDF_SMP_Enqueue,
      _Scheduler_EDF_SMP_Allocate_processor,
      _Scheduler_EDF_SMP_Get_idle,
      _Scheduler_EDF_SMP_Release_idle
    );
  }

  return STATUS_SUCCESSFUL;
}
