/**
 * @file
 *
 * @brief EDF SMP Scheduler Implementation
 *
 * @ingroup RTEMSScoreSchedulerSMPEDF
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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

static inline void _Scheduler_EDF_SMP_Set_scheduled(
  Scheduler_EDF_SMP_Context *self,
  Scheduler_EDF_SMP_Node    *scheduled,
  const Per_CPU_Control     *cpu
)
{
  self->Ready[ _Per_CPU_Get_index( cpu ) + 1 ].scheduled = scheduled;
}

static inline Scheduler_EDF_SMP_Node *_Scheduler_EDF_SMP_Get_scheduled(
  const Scheduler_EDF_SMP_Context *self,
  uint8_t                          rqi
)
{
  return self->Ready[ rqi ].scheduled;
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
    Scheduler_EDF_SMP_Node    *node;

    self = _Scheduler_EDF_SMP_Get_self( context );
    node = _Scheduler_EDF_SMP_Get_scheduled( self, rqi );

    if ( node->ready_queue_index > 0 ) {
      _Assert( node->ready_queue_index == rqi );
      return &node->Base.Base;
    }
  }

  return _Scheduler_SMP_Get_lowest_scheduled( context, filter_base );
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
  int                            generation_index;
  int                            increment;
  int64_t                        generation;

  self = _Scheduler_EDF_SMP_Get_self( context );
  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
  rqi = node->ready_queue_index;
  generation_index = SCHEDULER_PRIORITY_IS_APPEND( insert_priority );
  increment = ( generation_index << 1 ) - 1;
  ready_queue = &self->Ready[ rqi ];

  generation = self->generations[ generation_index ];
  node->generation = generation;
  self->generations[ generation_index ] = generation + increment;

  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Insert_inline(
    &ready_queue->Queue,
    &node->Base.Base.Node.RBTree,
    &insert_priority,
    _Scheduler_EDF_SMP_Priority_less_equal
  );

  if ( rqi != 0 && _Chain_Is_node_off_chain( &ready_queue->Node ) ) {
    Scheduler_EDF_SMP_Node *scheduled;

    scheduled = _Scheduler_EDF_SMP_Get_scheduled( self, rqi );

    if ( scheduled->ready_queue_index == 0 ) {
      _Chain_Append_unprotected( &self->Affine_queues, &ready_queue->Node );
    }
  }
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
      && !_Chain_Is_node_off_chain( &ready_queue->Node )
  ) {
    _Chain_Extract_unprotected( &ready_queue->Node );
    _Chain_Set_off_chain( &ready_queue->Node );
  }
}

static inline void _Scheduler_EDF_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Priority_Control insert_priority;

  _Scheduler_SMP_Extract_from_scheduled( context, scheduled_to_ready );
  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );
  _Scheduler_EDF_SMP_Insert_ready(
    context,
    scheduled_to_ready,
    insert_priority
  );
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
  _Scheduler_SMP_Insert_scheduled(
    context,
    ready_to_scheduled,
    insert_priority
  );
}

static inline void _Scheduler_EDF_SMP_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Scheduler_Node    *victim_base,
  Per_CPU_Control   *victim_cpu
)
{
  Scheduler_EDF_SMP_Context     *self;
  Scheduler_EDF_SMP_Node        *scheduled;
  uint8_t                        rqi;

  (void) victim_base;
  self = _Scheduler_EDF_SMP_Get_self( context );
  scheduled = _Scheduler_EDF_SMP_Node_downcast( scheduled_base );
  rqi = scheduled->ready_queue_index;

  if ( rqi != 0 ) {
    Scheduler_EDF_SMP_Ready_queue *ready_queue;
    Per_CPU_Control               *desired_cpu;

    ready_queue = &self->Ready[ rqi ];

    if ( !_Chain_Is_node_off_chain( &ready_queue->Node ) ) {
      _Chain_Extract_unprotected( &ready_queue->Node );
      _Chain_Set_off_chain( &ready_queue->Node );
    }

    desired_cpu = _Per_CPU_Get_by_index( rqi - 1 );

    if ( victim_cpu != desired_cpu ) {
      Scheduler_EDF_SMP_Node *node;

      node = _Scheduler_EDF_SMP_Get_scheduled( self, rqi );
      _Assert( node->ready_queue_index == 0 );
      _Scheduler_EDF_SMP_Set_scheduled( self, node, victim_cpu );
      _Scheduler_SMP_Allocate_processor_exact(
        context,
        &node->Base.Base,
        NULL,
        victim_cpu
      );
      victim_cpu = desired_cpu;
    }
  }

  _Scheduler_EDF_SMP_Set_scheduled( self, scheduled, victim_cpu );
  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &scheduled->Base.Base,
    NULL,
    victim_cpu
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
    _Scheduler_EDF_SMP_Allocate_processor
  );
}

static inline bool _Scheduler_EDF_SMP_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_EDF_SMP_Get_lowest_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor
  );
}

static inline bool _Scheduler_EDF_SMP_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor
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
    _Scheduler_EDF_SMP_Enqueue
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
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_EDF_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_EDF_SMP_Get_lowest_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor
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
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_EDF_SMP_Allocate_processor
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
  _Scheduler_EDF_SMP_Set_scheduled( self, idle, cpu );
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
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Enqueue
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
  rqi = (uint8_t) _Per_CPU_Get_index( cpu ) + 1;

  _Assert(
    _Scheduler_SMP_Node_state( &node->Base.Base ) == SCHEDULER_SMP_NODE_BLOCKED
  );

  node = _Scheduler_EDF_SMP_Node_downcast( node_base );
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

bool _Scheduler_EDF_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context      *context;
  Scheduler_EDF_SMP_Node *node;
  Processor_mask          local_affinity;
  uint8_t                 rqi;

  context = _Scheduler_Get_context( scheduler );
  _Processor_mask_And( &local_affinity, &context->Processors, affinity );

  if ( _Processor_mask_Is_zero( &local_affinity ) ) {
    return false;
  }

  if ( _Processor_mask_Is_equal( affinity, &_SMP_Online_processors ) ) {
    rqi = 0;
  } else {
    rqi = _Processor_mask_Find_last_set( &local_affinity );
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
      _Scheduler_EDF_SMP_Extract_from_ready,
      _Scheduler_EDF_SMP_Get_highest_ready,
      _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
      _Scheduler_EDF_SMP_Enqueue,
      _Scheduler_EDF_SMP_Allocate_processor
    );
  }

  return true;
}
