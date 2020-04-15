/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler Implementation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerstrongapa.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>

static Scheduler_strong_APA_Context *_Scheduler_strong_APA_Get_self(
  Scheduler_Context *context
)
{
  return (Scheduler_strong_APA_Context *) context;
}

static Scheduler_strong_APA_Node *
_Scheduler_strong_APA_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_strong_APA_Node *) node;
}

static void _Scheduler_strong_APA_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_self( context );
  Scheduler_strong_APA_Node *node =
    _Scheduler_strong_APA_Node_downcast( scheduled_to_ready );

  _Chain_Extract_unprotected( &node->Base.Base.Node.Chain );
  _Scheduler_priority_Ready_queue_enqueue_first(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static void _Scheduler_strong_APA_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *node;
  Priority_Control              insert_priority;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( ready_to_scheduled );

  _Scheduler_priority_Ready_queue_extract(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
  insert_priority = _Scheduler_SMP_Node_priority( &node->Base.Base );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Chain_Insert_ordered_unprotected(
    &self->Base.Scheduled,
    &node->Base.Base.Node.Chain,
    &insert_priority,
    _Scheduler_SMP_Priority_less_equal
  );
}

static void _Scheduler_strong_APA_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_base );

  if ( SCHEDULER_PRIORITY_IS_APPEND( insert_priority ) ) {
    _Scheduler_priority_Ready_queue_enqueue(
      &node->Base.Base.Node.Chain,
      &node->Ready_queue,
      &self->Bit_map
    );
  } else {
    _Scheduler_priority_Ready_queue_enqueue_first(
      &node->Base.Base.Node.Chain,
      &node->Ready_queue,
      &self->Bit_map
    );
  }
}

static void _Scheduler_strong_APA_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *the_thread
)
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_self( context );
  Scheduler_strong_APA_Node *node =
    _Scheduler_strong_APA_Node_downcast( the_thread );

  _Scheduler_priority_Ready_queue_extract(
    &node->Base.Base.Node.Chain,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static void _Scheduler_strong_APA_Do_update(
  Scheduler_Context *context,
  Scheduler_Node *node_to_update,
  Priority_Control new_priority
)
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_self( context );
  Scheduler_strong_APA_Node *node =
    _Scheduler_strong_APA_Node_downcast( node_to_update );

  _Scheduler_SMP_Node_update_priority( &node->Base, new_priority );
  _Scheduler_priority_Ready_queue_update(
    &node->Ready_queue,
    SCHEDULER_PRIORITY_UNMAP( new_priority ),
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_strong_APA_Context *) _Scheduler_Get_context( scheduler );
}

void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize(
    &self->Ready[ 0 ],
    scheduler->maximum_priority
  );
}

void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_Context            *context;
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *the_node;

  the_node = _Scheduler_strong_APA_Node_downcast( node );
  _Scheduler_SMP_Node_initialize(
    scheduler,
    &the_node->Base,
    the_thread,
    priority
  );

  context = _Scheduler_Get_context( scheduler );
  self = _Scheduler_strong_APA_Get_self( context );
  _Scheduler_priority_Ready_queue_update(
    &the_node->Ready_queue,
    SCHEDULER_PRIORITY_UNMAP( priority ),
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static bool _Scheduler_strong_APA_Has_ready( Scheduler_Context *context )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_self( context );

  return !_Priority_bit_map_Is_empty( &self->Bit_map );
}

static Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node
)
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_self( context );

  (void) node;

  return (Scheduler_Node *) _Scheduler_priority_Ready_queue_first(
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

static bool _Scheduler_strong_APA_Enqueue(
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
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

static bool _Scheduler_strong_APA_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control  insert_priority
)
{
  return _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    the_thread,
    node,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue
  );
}

static bool _Scheduler_strong_APA_Do_ask_for_help(
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
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    the_thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Do_ask_for_help
  );
}

bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_strong_APA_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_strong_APA_Reconsider_help_request(
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
    _Scheduler_strong_APA_Extract_from_ready
  );
}

void _Scheduler_strong_APA_Withdraw_node(
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
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_strong_APA_Has_ready,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}

Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue
  );
}

void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    the_thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled
  );
}
