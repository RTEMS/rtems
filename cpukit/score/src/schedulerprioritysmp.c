/**
 * @file
 *
 * @brief Deterministic Priority SMP Scheduler Implementation
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerprioritysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/wkspace.h>

static Scheduler_priority_SMP_Control *
_Scheduler_priority_SMP_Self_from_base( Scheduler_Control *base )
{
  return (Scheduler_priority_SMP_Control *) base->information;
}

static Scheduler_priority_SMP_Control *
_Scheduler_priority_SMP_Self_from_SMP_base( Scheduler_SMP_Control *smp_base )
{
  return (Scheduler_priority_SMP_Control *)
    ( (char *) smp_base
      - offsetof( Scheduler_priority_SMP_Control, Base ) );
}

void _Scheduler_priority_SMP_Initialize( void )
{
  Scheduler_priority_SMP_Control *self = _Workspace_Allocate_or_fatal_error(
    sizeof( *self ) + PRIORITY_MAXIMUM * sizeof( Chain_Control )
  );

  _Scheduler_SMP_Initialize( &self->Base );
  _Priority_bit_map_Initialize( &self->Bit_map );
  _Scheduler_priority_Ready_queue_initialize( &self->Ready[ 0 ] );

  _Scheduler.information = self;
}

void _Scheduler_priority_SMP_Update(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_priority_Update_body(
    thread,
    &self->Bit_map,
    &self->Ready[ 0 ]
  );
}

static Thread_Control *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_SMP_Control *smp_base
)
{
  Scheduler_priority_SMP_Control *self =
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
  Scheduler_SMP_Control *smp_base,
  Thread_Control *scheduled_to_ready
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );

  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_priority_Ready_queue_enqueue_first(
    scheduled_to_ready,
    &self->Bit_map
  );
}

static void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *ready_to_scheduled
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );

  _Scheduler_priority_Ready_queue_extract(
    ready_to_scheduled,
    &self->Bit_map
  );
  _Scheduler_simple_Insert_priority_fifo(
    &self->Base.Scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_priority_SMP_Insert_ready_lifo(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );

  _Scheduler_priority_Ready_queue_enqueue( thread, &self->Bit_map );
}

static void _Scheduler_priority_SMP_Insert_ready_fifo(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );

  _Scheduler_priority_Ready_queue_enqueue_first( thread, &self->Bit_map );
}

static void _Scheduler_priority_SMP_Do_extract(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_SMP_base( smp_base );
  bool is_scheduled = thread->is_scheduled;

  thread->is_in_the_air = is_scheduled;
  thread->is_scheduled = false;

  if ( is_scheduled ) {
    _Chain_Extract_unprotected( &thread->Object.Node );
  } else {
    _Scheduler_priority_Ready_queue_extract( thread, &self->Bit_map );
  }
}

void _Scheduler_priority_SMP_Block(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_SMP_Block(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Do_extract,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled
  );
}

static void _Scheduler_priority_SMP_Enqueue_ordered(
  Scheduler_SMP_Control *self,
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
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_priority_SMP_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

void _Scheduler_priority_SMP_Enqueue_fifo(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_priority_SMP_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_priority_SMP_Extract(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_SMP_Extract(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Do_extract
  );
}

void _Scheduler_priority_SMP_Yield(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_priority_SMP_Extract( base, thread );
  _Scheduler_priority_SMP_Enqueue_fifo( base, thread );

  _ISR_Enable( level );
}

void _Scheduler_priority_SMP_Schedule(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_SMP_Schedule(
    &self->Base,
    thread,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled
  );
}

void _Scheduler_priority_SMP_Start_idle(
  Scheduler_Control *base,
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_priority_SMP_Control *self =
    _Scheduler_priority_SMP_Self_from_base( base );

  _Scheduler_SMP_Start_idle( &self->Base, thread, cpu );
}
