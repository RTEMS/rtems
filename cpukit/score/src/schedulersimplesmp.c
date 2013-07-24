/**
 * @file
 *
 * @brief Simple SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulersimplesmp.h>
#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/score/wkspace.h>

static Scheduler_simple_smp_Control *_Scheduler_simple_smp_Instance( void )
{
  return _Scheduler.information;
}

void _Scheduler_simple_smp_Initialize( void )
{
  Scheduler_simple_smp_Control *self =
    _Workspace_Allocate_or_fatal_error( sizeof( *self ) );

  _Chain_Initialize_empty( &self->ready );
  _Chain_Initialize_empty( &self->scheduled );

  _Scheduler.information = self;
}

static void _Scheduler_simple_smp_Allocate_processor(
  Thread_Control *scheduled,
  Thread_Control *victim
)
{
  Per_CPU_Control *cpu_of_scheduled = scheduled->cpu;
  Per_CPU_Control *cpu_of_victim = victim->cpu;
  Thread_Control *heir;

  scheduled->is_scheduled = true;
  victim->is_scheduled = false;

  if ( scheduled->is_executing ) {
    heir = cpu_of_scheduled->heir;
    cpu_of_scheduled->heir = scheduled;
  } else {
    heir = scheduled;
  }

  if ( heir != victim ) {
    heir->cpu = cpu_of_victim;
    cpu_of_victim->heir = heir;
    cpu_of_victim->dispatch_necessary = true;
  }
}

static void _Scheduler_simple_smp_Move_from_scheduled_to_ready(
  Chain_Control *ready_chain,
  Thread_Control *scheduled_to_ready
)
{
  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_simple_Insert_priority_lifo( ready_chain, scheduled_to_ready );
}

static void _Scheduler_simple_smp_Move_from_ready_to_scheduled(
  Chain_Control *scheduled_chain,
  Thread_Control *ready_to_scheduled
)
{
  _Chain_Extract_unprotected( &ready_to_scheduled->Object.Node );
  _Scheduler_simple_Insert_priority_fifo( scheduled_chain, ready_to_scheduled );
}

static void _Scheduler_simple_smp_Insert(
  Chain_Control *chain,
  Thread_Control *thread,
  Chain_Node_order order
)
{
  _Chain_Insert_ordered_unprotected( chain, &thread->Object.Node, order );
}

static void _Scheduler_simple_smp_Enqueue_ordered(
  Thread_Control *thread,
  Chain_Node_order order
)
{
  Scheduler_simple_smp_Control *self = _Scheduler_simple_smp_Instance();

  /*
   * The scheduled chain has exactly processor count nodes after
   * initialization, thus the lowest priority scheduled thread exists.
   */
  Thread_Control *lowest_scheduled =
    (Thread_Control *) _Chain_Last( &self->scheduled );

  if ( ( *order )( &thread->Object.Node, &lowest_scheduled->Object.Node ) ) {
    _Scheduler_simple_smp_Allocate_processor( thread, lowest_scheduled );

    _Scheduler_simple_smp_Insert( &self->scheduled, thread, order );

    _Scheduler_simple_smp_Move_from_scheduled_to_ready(
      &self->ready,
      lowest_scheduled
    );
  } else {
    _Scheduler_simple_smp_Insert( &self->ready, thread, order );
  }
}

void _Scheduler_simple_smp_Enqueue_priority_lifo( Thread_Control *thread )
{
  _Scheduler_simple_smp_Enqueue_ordered(
    thread,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

void _Scheduler_simple_smp_Enqueue_priority_fifo( Thread_Control *thread )
{
  _Scheduler_simple_smp_Enqueue_ordered(
    thread,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

void _Scheduler_simple_smp_Extract( Thread_Control *thread )
{
  Scheduler_simple_smp_Control *self = _Scheduler_simple_smp_Instance();

  _Chain_Extract_unprotected( &thread->Object.Node );

  if ( thread->is_scheduled ) {
    Thread_Control *highest_ready =
      (Thread_Control *) _Chain_First( &self->ready );

    _Scheduler_simple_smp_Allocate_processor( highest_ready, thread );

    _Scheduler_simple_smp_Move_from_ready_to_scheduled(
      &self->scheduled,
      highest_ready
    );
  }
}

void _Scheduler_simple_smp_Yield( Thread_Control *thread )
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_simple_smp_Extract( thread );
  _Scheduler_simple_smp_Enqueue_priority_fifo( thread );

  _ISR_Enable( level );
}

void _Scheduler_simple_smp_Schedule( void )
{
  /* Nothing to do */
}

void _Scheduler_simple_smp_Start_idle(
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_simple_smp_Control *self = _Scheduler_simple_smp_Instance();

  thread->is_scheduled = true;
  thread->cpu = cpu;
  _Chain_Append_unprotected( &self->scheduled, &thread->Object.Node );
}
