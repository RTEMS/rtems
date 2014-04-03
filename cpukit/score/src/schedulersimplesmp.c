/**
 * @file
 *
 * @brief Simple SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
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
#include <rtems/score/wkspace.h>

static Scheduler_simple_SMP_Control *
_Scheduler_simple_SMP_Self_from_base( Scheduler_Control *base )
{
  return (Scheduler_simple_SMP_Control *) base->information;
}

static Scheduler_simple_SMP_Control *
_Scheduler_simple_SMP_Self_from_SMP_base( Scheduler_SMP_Control *smp_base )
{
  return (Scheduler_simple_SMP_Control *)
    ( (char *) smp_base - offsetof( Scheduler_simple_SMP_Control, Base ) );
}

void _Scheduler_simple_smp_Initialize( void )
{
  Scheduler_simple_SMP_Control *self =
    _Workspace_Allocate_or_fatal_error( sizeof( *self ) );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->Ready );

  _Scheduler.information = self;
}

static Thread_Control *_Scheduler_simple_smp_Get_highest_ready(
  Scheduler_SMP_Control *smp_base
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_SMP_base( smp_base );
  Thread_Control *highest_ready = NULL;
  Chain_Control *ready = &self->Ready;

  if ( !_Chain_Is_empty( ready ) ) {
    highest_ready = (Thread_Control *) _Chain_First( ready );
  }

  return highest_ready;
}

static void _Scheduler_simple_smp_Move_from_scheduled_to_ready(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *scheduled_to_ready
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_SMP_base( smp_base );

  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_simple_Insert_priority_lifo(
    &self->Ready,
    scheduled_to_ready
  );
}

static void _Scheduler_simple_smp_Move_from_ready_to_scheduled(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *ready_to_scheduled
)
{
  _Chain_Extract_unprotected( &ready_to_scheduled->Object.Node );
  _Scheduler_simple_Insert_priority_fifo(
    &smp_base->Scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_simple_smp_Insert_ready_lifo(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_SMP_base( smp_base );

  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static void _Scheduler_simple_smp_Insert_ready_fifo(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_SMP_base( smp_base );

  _Chain_Insert_ordered_unprotected(
    &self->Ready,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

static void _Scheduler_simple_smp_Do_extract(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread
)
{
  (void) smp_base;

  thread->is_in_the_air = thread->is_scheduled;
  thread->is_scheduled = false;

  _Chain_Extract_unprotected( &thread->Object.Node );
}

void _Scheduler_simple_smp_Block(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_SMP_Block(
    &self->Base,
    thread,
    _Scheduler_simple_smp_Do_extract,
    _Scheduler_simple_smp_Get_highest_ready,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled
  );
}

static void _Scheduler_simple_smp_Enqueue_ordered(
  Scheduler_SMP_Control *smp_base,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled
)
{
  _Scheduler_SMP_Enqueue_ordered(
    smp_base,
    thread,
    order,
    _Scheduler_simple_smp_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled,
    _Scheduler_simple_smp_Move_from_scheduled_to_ready
  );
}

void _Scheduler_simple_smp_Enqueue_priority_lifo(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_simple_smp_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_simple_smp_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

void _Scheduler_simple_smp_Enqueue_priority_fifo(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_simple_smp_Enqueue_ordered(
    &self->Base,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_simple_smp_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_simple_smp_Extract(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_SMP_Extract(
    &self->Base,
    thread,
    _Scheduler_simple_smp_Do_extract
  );
}

void _Scheduler_simple_smp_Yield(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_simple_smp_Extract( base, thread );
  _Scheduler_simple_smp_Enqueue_priority_fifo( base, thread );

  _ISR_Enable( level );
}

void _Scheduler_simple_smp_Schedule(
  Scheduler_Control *base,
  Thread_Control *thread
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_SMP_Schedule(
    &self->Base,
    thread,
    _Scheduler_simple_smp_Get_highest_ready,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled
  );
}

void _Scheduler_simple_smp_Start_idle(
  Scheduler_Control *base,
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_simple_SMP_Control *self =
    _Scheduler_simple_SMP_Self_from_base( base );

  _Scheduler_SMP_Start_idle( &self->Base, thread, cpu );
}
