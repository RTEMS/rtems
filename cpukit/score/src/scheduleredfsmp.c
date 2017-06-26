/**
 * @file
 *
 * @brief EDF SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMPEDF
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

static Scheduler_EDF_SMP_Context *
_Scheduler_EDF_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_EDF_SMP_Context *) _Scheduler_Get_context( scheduler );
}

static Scheduler_EDF_SMP_Context *
_Scheduler_EDF_SMP_Get_self( Scheduler_Context *context )
{
  return (Scheduler_EDF_SMP_Context *) context;
}

static inline Scheduler_EDF_SMP_Node *
_Scheduler_EDF_SMP_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_EDF_SMP_Node *) node;
}

static inline bool _Scheduler_EDF_SMP_Less(
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

  return prio_left < prio_right;
}

static inline bool _Scheduler_EDF_SMP_Less_or_equal(
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
  _RBTree_Initialize_empty( &self->Ready );
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

static void _Scheduler_EDF_SMP_Do_update(
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

static bool _Scheduler_EDF_SMP_Has_ready( Scheduler_Context *context )
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );

  return !_RBTree_Is_empty( &self->Ready );
}

static Scheduler_Node *_Scheduler_EDF_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_Node *first = (Scheduler_Node *) _RBTree_Minimum( &self->Ready );

  (void) node;

  _Assert( &first->Node != NULL );

  return first;
}

static void _Scheduler_EDF_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_EDF_SMP_Node *node =
    _Scheduler_EDF_SMP_Node_downcast( scheduled_to_ready );

  _Chain_Extract_unprotected( &node->Base.Base.Node.Chain );
  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Insert_inline(
    &self->Ready,
    &node->Base.Base.Node.RBTree,
    &node->Base.priority,
    _Scheduler_EDF_SMP_Less
  );
}

static void _Scheduler_EDF_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_EDF_SMP_Node *node =
    _Scheduler_EDF_SMP_Node_downcast( ready_to_scheduled );

  _RBTree_Extract( &self->Ready, &node->Base.Base.Node.RBTree );
  _Chain_Initialize_node( &node->Base.Base.Node.Chain );
  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &node->Base.Base.Node.Chain,
    _Scheduler_SMP_Insert_priority_fifo_order
  );
}

static void _Scheduler_EDF_SMP_Insert_ready_lifo(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_EDF_SMP_Node *node =
    _Scheduler_EDF_SMP_Node_downcast( node_to_insert );

  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Insert_inline(
    &self->Ready,
    &node->Base.Base.Node.RBTree,
    &node->Base.priority,
    _Scheduler_EDF_SMP_Less_or_equal
  );
}

static void _Scheduler_EDF_SMP_Insert_ready_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_EDF_SMP_Node *node =
    _Scheduler_EDF_SMP_Node_downcast( node_to_insert );

  _RBTree_Initialize_node( &node->Base.Base.Node.RBTree );
  _RBTree_Insert_inline(
    &self->Ready,
    &node->Base.Base.Node.RBTree,
    &node->Base.priority,
    _Scheduler_EDF_SMP_Less
  );
}

static void _Scheduler_EDF_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_EDF_SMP_Context *self =
    _Scheduler_EDF_SMP_Get_self( context );
  Scheduler_EDF_SMP_Node *node =
    _Scheduler_EDF_SMP_Node_downcast( node_to_extract );

  _RBTree_Extract( &self->Ready, &node->Base.Base.Node.RBTree );
  _Chain_Initialize_node( &node->Base.Base.Node.Chain );
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
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_ordered(
  Scheduler_Context    *context,
  Scheduler_Node       *node,
  Chain_Node_order      order,
  Scheduler_SMP_Insert  insert_ready,
  Scheduler_SMP_Insert  insert_scheduled
)
{
  return _Scheduler_SMP_Enqueue_ordered(
    context,
    node,
    order,
    insert_ready,
    insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_lifo(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  return _Scheduler_EDF_SMP_Enqueue_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_EDF_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  return _Scheduler_EDF_SMP_Enqueue_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_fifo_order,
    _Scheduler_EDF_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_scheduled_ordered(
  Scheduler_Context *context,
  Scheduler_Node *node,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  return _Scheduler_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    order,
    _Scheduler_EDF_SMP_Extract_from_ready,
    _Scheduler_EDF_SMP_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_EDF_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_scheduled_lifo(
  Scheduler_Context *context,
  Scheduler_Node *node
)
{
  return _Scheduler_EDF_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_EDF_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static bool _Scheduler_EDF_SMP_Enqueue_scheduled_fifo(
  Scheduler_Context *context,
  Scheduler_Node *node
)
{
  return _Scheduler_EDF_SMP_Enqueue_scheduled_ordered(
    context,
    node,
    _Scheduler_SMP_Insert_priority_fifo_order,
    _Scheduler_EDF_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
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
    _Scheduler_EDF_SMP_Enqueue_fifo
  );
}

static bool _Scheduler_EDF_SMP_Do_ask_for_help(
  Scheduler_Context *context,
  Thread_Control    *the_thread,
  Scheduler_Node    *node
)
{
  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Insert_priority_lifo_order,
    _Scheduler_EDF_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo,
    _Scheduler_EDF_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
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
    _Scheduler_EDF_SMP_Enqueue_fifo,
    _Scheduler_EDF_SMP_Enqueue_lifo,
    _Scheduler_EDF_SMP_Enqueue_scheduled_fifo,
    _Scheduler_EDF_SMP_Enqueue_scheduled_lifo,
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
    _Scheduler_SMP_Allocate_processor_lazy
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
    _Scheduler_EDF_SMP_Enqueue_scheduled_fifo
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
    _Scheduler_EDF_SMP_Enqueue_fifo
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
    _Scheduler_EDF_SMP_Enqueue_fifo,
    _Scheduler_EDF_SMP_Enqueue_scheduled_fifo
  );
}
