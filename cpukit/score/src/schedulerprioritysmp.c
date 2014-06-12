/**
 * @file
 *
 * @ingroup ScoreSchedulerPrioritySMP
 *
 * @brief Deterministic Priority SMP Scheduler Implementation
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerprioritysmpimpl.h>
#include <rtems/score/schedulersmpimpl.h>

static Scheduler_priority_SMP_Context *
_Scheduler_priority_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_SMP_Context *) _Scheduler_Get_context( scheduler );
}

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize( &self->Ready[ 0 ] );
}

void _Scheduler_priority_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  _Scheduler_SMP_Node_initialize( node );
}

void _Scheduler_priority_SMP_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  Priority_Control new_priority
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );
  Scheduler_Node *node = _Scheduler_Node_get( thread );

  _Scheduler_priority_SMP_Do_update( context, node, new_priority );
}

static Thread_Control *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Thread_Control    *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_self( context );

  (void) thread;

  return _Scheduler_priority_Ready_queue_first(
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor
  );
}

static void _Scheduler_priority_SMP_Enqueue_ordered(
  Scheduler_Context *context,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  _Scheduler_SMP_Enqueue_ordered(
    context,
    thread,
    order,
    insert_ready,
    insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor
  );
}

static void _Scheduler_priority_SMP_Enqueue_lifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  _Scheduler_priority_SMP_Enqueue_ordered(
    context,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static void _Scheduler_priority_SMP_Enqueue_fifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  _Scheduler_priority_SMP_Enqueue_ordered(
    context,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

static void _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
  Scheduler_Context *context,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  _Scheduler_SMP_Enqueue_scheduled_ordered(
    context,
    thread,
    order,
    _Scheduler_priority_SMP_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor
  );
}

static void _Scheduler_priority_SMP_Enqueue_scheduled_lifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
    context,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static void _Scheduler_priority_SMP_Enqueue_scheduled_fifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  _Scheduler_priority_SMP_Enqueue_scheduled_ordered(
    context,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_priority_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    _Scheduler_priority_SMP_Enqueue_fifo
  );
}

void _Scheduler_priority_SMP_Change_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Priority_Control         new_priority,
  bool                     prepend_it
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Change_priority(
    context,
    thread,
    new_priority,
    prepend_it,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_SMP_Enqueue_fifo,
    _Scheduler_priority_SMP_Enqueue_lifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_fifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_lifo
  );
}

void _Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Yield(
    context,
    thread,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Enqueue_fifo,
    _Scheduler_priority_SMP_Enqueue_scheduled_fifo
  );
}
