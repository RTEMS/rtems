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

#include <rtems/score/schedulerprioritysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/wkspace.h>

static Scheduler_priority_SMP_Context *
_Scheduler_priority_SMP_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_SMP_Context *) scheduler->context;
}

static Scheduler_priority_SMP_Context *
_Scheduler_priority_SMP_Self_from_SMP_base( Scheduler_SMP_Context *smp_base )
{
  return (Scheduler_priority_SMP_Context *)
    ( (char *) smp_base
      - offsetof( Scheduler_priority_SMP_Context, Base ) );
}

static Scheduler_priority_SMP_Node *_Scheduler_priority_SMP_Node_get(
  Thread_Control *thread
)
{
  return (Scheduler_priority_SMP_Node *) _Scheduler_Node_get( thread );
}

void _Scheduler_priority_SMP_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize( &self->Ready[ 0 ] );
}

bool _Scheduler_priority_SMP_Allocate(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  _Scheduler_SMP_Node_initialize( node );

  return true;
}

void _Scheduler_priority_SMP_Update(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( thread );

  _Scheduler_priority_Ready_queue_update(
    thread,
    &node->Ready_queue,
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static Thread_Control *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_SMP_Context *smp_base
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Thread_Control *highest_ready = NULL;

  if ( !_Priority_bit_map_Is_empty( &self->Bit_map ) ) {
    highest_ready = _Scheduler_priority_Ready_queue_first(
      &self->Bit_map,
      &self->Ready[ 0 ]
    );
  }

  return highest_ready;
}

static void _Scheduler_priority_SMP_Move_from_scheduled_to_ready(
  Scheduler_SMP_Context *smp_base,
  Thread_Control *scheduled_to_ready
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( scheduled_to_ready );

  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_priority_Ready_queue_enqueue_first(
    scheduled_to_ready,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_SMP_Context *smp_base,
  Thread_Control *ready_to_scheduled
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( ready_to_scheduled );

  _Scheduler_priority_Ready_queue_extract(
    ready_to_scheduled,
    &node->Ready_queue,
    &self->Bit_map
  );
  _Scheduler_simple_Insert_priority_fifo(
    &self->Base.Scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_priority_SMP_Insert_ready_lifo(
  Scheduler_SMP_Context *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( thread );

  _Scheduler_priority_Ready_queue_enqueue(
    thread,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static void _Scheduler_priority_SMP_Insert_ready_fifo(
  Scheduler_SMP_Context *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( thread );

  _Scheduler_priority_Ready_queue_enqueue_first(
    thread,
    &node->Ready_queue,
    &self->Bit_map
  );
}

static void _Scheduler_priority_SMP_Do_extract(
  Scheduler_SMP_Context *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  Scheduler_priority_SMP_Node *node =
    _Scheduler_priority_SMP_Node_get( thread );

  if ( node->Base.state == SCHEDULER_SMP_NODE_SCHEDULED ) {
    _Scheduler_SMP_Node_change_state(
      &node->Base,
      SCHEDULER_SMP_NODE_IN_THE_AIR
    );
    _Chain_Extract_unprotected( &thread->Object.Node );
  } else {
    _Scheduler_SMP_Node_change_state(
      &node->Base,
      SCHEDULER_SMP_NODE_BLOCKED
    );
    _Scheduler_priority_Ready_queue_extract(
      thread,
      &node->Ready_queue,
      &self->Bit_map
    );
  }
}

void _Scheduler_priority_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Block(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Do_extract,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled
  );
}

static void _Scheduler_priority_SMP_Enqueue_ordered(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  _Scheduler_SMP_Enqueue_ordered(
    self,
    thread,
    order,
    _Scheduler_priority_SMP_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready
  );
}

void _Scheduler_priority_SMP_Enqueue_lifo(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_priority_SMP_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

void _Scheduler_priority_SMP_Enqueue_fifo(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_priority_SMP_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_priority_SMP_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Extract(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Do_extract
  );
}

void _Scheduler_priority_SMP_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_priority_SMP_Extract( scheduler, thread );
  _Scheduler_priority_SMP_Enqueue_fifo( scheduler, thread );

  _ISR_Enable( level );
}

void _Scheduler_priority_SMP_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Schedule(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled
  );
}

void _Scheduler_priority_SMP_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_priority_SMP_Context *self =
    _Scheduler_priority_SMP_Get_context( scheduler );

  _Scheduler_SMP_Start_idle( &self->Base, thread, cpu );
}
