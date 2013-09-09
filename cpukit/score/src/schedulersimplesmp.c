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
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/wkspace.h>

void _Scheduler_simple_smp_Initialize( void )
{
  Scheduler_SMP_Control *self =
    _Workspace_Allocate_or_fatal_error( sizeof( *self ) );

  _Chain_Initialize_empty( &self->ready[ 0 ] );
  _Chain_Initialize_empty( &self->scheduled );

  _Scheduler.information = self;
}

static Thread_Control *_Scheduler_simple_smp_Get_highest_ready(
  Scheduler_SMP_Control *self
)
{
  Thread_Control *highest_ready = NULL;
  Chain_Control *ready = &self->ready[ 0 ];

  if ( !_Chain_Is_empty( ready ) ) {
    highest_ready = (Thread_Control *) _Chain_First( ready );
  }

  return highest_ready;
}

static void _Scheduler_simple_smp_Move_from_scheduled_to_ready(
  Scheduler_SMP_Control *self,
  Thread_Control *scheduled_to_ready
)
{
  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_simple_Insert_priority_lifo(
    &self->ready[ 0 ],
    scheduled_to_ready
  );
}

static void _Scheduler_simple_smp_Move_from_ready_to_scheduled(
  Scheduler_SMP_Control *self,
  Thread_Control *ready_to_scheduled
)
{
  _Chain_Extract_unprotected( &ready_to_scheduled->Object.Node );
  _Scheduler_simple_Insert_priority_fifo(
    &self->scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_simple_smp_Insert_ready_lifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->ready[ 0 ],
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static void _Scheduler_simple_smp_Insert_ready_fifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->ready[ 0 ],
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

static void _Scheduler_simple_smp_Do_extract(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  ( void ) self;

  thread->is_in_the_air = thread->is_scheduled;
  thread->is_scheduled = false;

  _Chain_Extract_unprotected( &thread->Object.Node );
}

void _Scheduler_simple_smp_Block( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Block(
    self,
    thread,
    _Scheduler_simple_smp_Do_extract,
    _Scheduler_simple_smp_Get_highest_ready,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled
  );
}

static void _Scheduler_simple_smp_Enqueue_ordered(
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
    _Scheduler_simple_smp_Get_highest_ready,
    insert_ready,
    insert_scheduled,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled,
    _Scheduler_simple_smp_Move_from_scheduled_to_ready
  );
}

void _Scheduler_simple_smp_Enqueue_priority_lifo( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_simple_smp_Enqueue_ordered(
    self,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_simple_smp_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

void _Scheduler_simple_smp_Enqueue_priority_fifo( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_simple_smp_Enqueue_ordered(
    self,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_simple_smp_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_simple_smp_Extract( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Extract(
    self,
    thread,
    _Scheduler_simple_smp_Do_extract
  );
}

void _Scheduler_simple_smp_Yield( Thread_Control *thread )
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_simple_smp_Extract( thread );
  _Scheduler_simple_smp_Enqueue_priority_fifo( thread );

  _ISR_Enable( level );
}

void _Scheduler_simple_smp_Schedule( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Schedule(
    self,
    thread,
    _Scheduler_simple_smp_Get_highest_ready,
    _Scheduler_simple_smp_Move_from_ready_to_scheduled
  );
}
