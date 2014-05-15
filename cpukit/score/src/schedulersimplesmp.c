/**
 * @file
 *
 * @brief Simple SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMPSimple
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.
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

bool _Scheduler_simple_SMP_Allocate(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( the_thread );

  _Scheduler_SMP_Node_initialize( node );

  return true;
}

static void _Scheduler_simple_SMP_Do_update(
  Scheduler_Context *context,
  Scheduler_Node *node,
  Priority_Control new_priority
)
{
  (void) context;
  (void) node;
  (void) new_priority;
}

static Thread_Control *_Scheduler_simple_SMP_Get_highest_ready(
  Scheduler_Context *context
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  return (Thread_Control *) _Chain_First( &self->Ready );
}

static void _Scheduler_simple_SMP_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Thread_Control *scheduled_to_ready
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_simple_Insert_priority_lifo(
    &self->Ready,
    scheduled_to_ready
  );
}

static void _Scheduler_simple_SMP_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Thread_Control *ready_to_scheduled
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  _Chain_Extract_unprotected( &ready_to_scheduled->Object.Node );
  _Scheduler_simple_Insert_priority_fifo(
    &self->Base.Scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_simple_SMP_Insert_ready_lifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static void _Scheduler_simple_SMP_Insert_ready_fifo(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Context *self =
    _Scheduler_simple_SMP_Get_self( context );

  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

static void _Scheduler_simple_SMP_Extract_from_ready(
  Scheduler_Context *context,
  Thread_Control *thread
)
{
  (void) context;

  _Chain_Extract_unprotected( &thread->Object.Node );
}

void _Scheduler_simple_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Get_highest_ready,
    _Scheduler_simple_SMP_Move_from_ready_to_scheduled
  );
}

static void _Scheduler_simple_SMP_Enqueue_ordered(
  Scheduler_Context *context,
  Thread_Control *thread,
  bool has_processor_allocated,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  _Scheduler_SMP_Enqueue_ordered(
    context,
    thread,
    has_processor_allocated,
    order,
    _Scheduler_simple_SMP_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_simple_SMP_Move_from_ready_to_scheduled,
    _Scheduler_simple_SMP_Move_from_scheduled_to_ready
  );
}

static void _Scheduler_simple_SMP_Enqueue_lifo(
  Scheduler_Context *context,
  Thread_Control *thread,
  bool has_processor_allocated
)
{
  _Scheduler_simple_SMP_Enqueue_ordered(
    context,
    thread,
    has_processor_allocated,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_simple_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

static void _Scheduler_simple_SMP_Enqueue_fifo(
  Scheduler_Context *context,
  Thread_Control *thread,
  bool has_processor_allocated
)
{
  _Scheduler_simple_SMP_Enqueue_ordered(
    context,
    thread,
    has_processor_allocated,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_simple_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_simple_SMP_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_simple_SMP_Enqueue_fifo( context, thread, false );
}

void _Scheduler_simple_SMP_Change_priority(
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
    _Scheduler_simple_SMP_Extract_from_ready,
    _Scheduler_simple_SMP_Do_update,
    _Scheduler_simple_SMP_Enqueue_fifo,
    _Scheduler_simple_SMP_Enqueue_lifo
  );
}

void _Scheduler_simple_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_SMP_Extract_from_scheduled( thread );
  _Scheduler_simple_SMP_Enqueue_fifo( context, thread, true );

  _ISR_Enable( level );
}

void _Scheduler_simple_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Start_idle( context, thread, cpu );
}
