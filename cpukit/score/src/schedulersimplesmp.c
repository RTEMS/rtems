/**
 * @file
 *
 * @brief Simple SMP Scheduler Implementation
 *
 * @ingroup RTEMSScoreSchedulerSMPSimple
 */

/*
 * Copyright (c) 2013, 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulersimplesmp.h>
#include <rtems/score/schedulersmpimpl.h>

static Scheduler_simple_SMP_Context *
_Scheduler_simple_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_simple_SMP_Context *) _Scheduler_Get_context( scheduler );
}

static Scheduler_simple_SMP_Context *
_Scheduler_simple_SMP_Get_self( Scheduler_Context *context )
{
  return (Scheduler_simple_SMP_Context *) context;
}

void _Scheduler_simple_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->Ready );
}

void _Scheduler_simple_SMP_Node_initialize(
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

static void _Scheduler_simple_SMP_Do_update(
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

static bool _Scheduler_simple_SMP_Has_ready( Scheduler_Context *context )
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  return !_Chain_Is_empty( &self->Ready );
}

static Scheduler_Node *_Scheduler_simple_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );
  Scheduler_Node *first = (Scheduler_Node *) _Chain_First( &self->Ready );

  (void) node;

  _Assert( &first->Node.Chain != _Chain_Tail( &self->Ready ) );

  return first;
}

static void _Scheduler_simple_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Scheduler_simple_SMP_Context *self;
  Priority_Control              insert_priority;

  self = _Scheduler_simple_SMP_Get_self( context );

  _Chain_Extract_unprotected( &scheduled_to_ready->Node.Chain );
  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );
  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &scheduled_to_ready->Node.Chain,
    &insert_priority,
    _Scheduler_SMP_Priority_less_equal
  );
}

static void _Scheduler_simple_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Scheduler_simple_SMP_Context *self;
  Priority_Control              insert_priority;

  self = _Scheduler_simple_SMP_Get_self( context );

  _Chain_Extract_unprotected( &ready_to_scheduled->Node.Chain );
  insert_priority = _Scheduler_SMP_Node_priority( ready_to_scheduled );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &ready_to_scheduled->Node.Chain,
    &insert_priority,
    _Scheduler_SMP_Priority_less_equal
  );
}

static void _Scheduler_simple_SMP_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_insert,
  Priority_Control   insert_priority
)
{
  Scheduler_simple_SMP_Context *self;

  self = _Scheduler_simple_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &node_to_insert->Node.Chain,
    &insert_priority,
    _Scheduler_SMP_Priority_less_equal
  );
}

static void _Scheduler_simple_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  (void) context;

  _Chain_Extract_unprotected( &node_to_extract->Node.Chain );
}

void _Scheduler_simple_SMP_Block(
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
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Get_highest_ready,
    _Scheduler_simple_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_simple_SMP_Enqueue(
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
    _Scheduler_simple_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_simple_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

static bool _Scheduler_simple_SMP_Enqueue_scheduled(
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
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Get_highest_ready,
    _Scheduler_simple_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_simple_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_simple_SMP_Unblock(
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
    _Scheduler_simple_SMP_Do_update,
    _Scheduler_simple_SMP_Enqueue
  );
}

static bool _Scheduler_simple_SMP_Do_ask_for_help(
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
    _Scheduler_simple_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_simple_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_simple_SMP_Update_priority(
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
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Do_update,
    _Scheduler_simple_SMP_Enqueue,
    _Scheduler_simple_SMP_Enqueue_scheduled,
    _Scheduler_simple_SMP_Do_ask_for_help
  );
}

bool _Scheduler_simple_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_simple_SMP_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_simple_SMP_Reconsider_help_request(
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
    _Scheduler_simple_SMP_Extract_from_ready
  );
}

void _Scheduler_simple_SMP_Withdraw_node(
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
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Get_highest_ready,
    _Scheduler_simple_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_simple_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_simple_SMP_Has_ready,
    _Scheduler_simple_SMP_Enqueue_scheduled,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}

Thread_Control *_Scheduler_simple_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Enqueue
  );
}

void _Scheduler_simple_SMP_Yield(
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
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Enqueue,
    _Scheduler_simple_SMP_Enqueue_scheduled
  );
}
