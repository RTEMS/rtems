/**
 * @file
 *
 * @brief Deterministic Priority SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerSMP
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerprioritysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/wkspace.h>

void _Scheduler_priority_SMP_Initialize( void )
{
  Scheduler_SMP_Control *self = _Workspace_Allocate_or_fatal_error(
    sizeof( *self ) + PRIORITY_MAXIMUM * sizeof( Chain_Control )
  );

  _Chain_Initialize_empty( &self->scheduled );
  _Scheduler_priority_Ready_queue_initialize( &self->ready[ 0 ] );

  _Scheduler.information = self;
}

void _Scheduler_priority_SMP_Update( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_priority_Update_body( thread, &self->ready[ 0 ] );
}

static Thread_Control *_Scheduler_priority_SMP_Get_highest_ready(
  Scheduler_SMP_Control *self
)
{
  Thread_Control *highest_ready = NULL;

  if ( !_Priority_bit_map_Is_empty() ) {
    highest_ready = _Scheduler_priority_Ready_queue_first( &self->ready[ 0 ] );
  }

  return highest_ready;
}

static void _Scheduler_priority_SMP_Move_from_scheduled_to_ready(
  Scheduler_SMP_Control *self,
  Thread_Control *scheduled_to_ready
)
{
  _Chain_Extract_unprotected( &scheduled_to_ready->Object.Node );
  _Scheduler_priority_Ready_queue_enqueue_first( scheduled_to_ready );
}

static void _Scheduler_priority_SMP_Move_from_ready_to_scheduled(
  Scheduler_SMP_Control *self,
  Thread_Control *ready_to_scheduled
)
{
  _Scheduler_priority_Ready_queue_extract( ready_to_scheduled );
  _Scheduler_simple_Insert_priority_fifo(
    &self->scheduled,
    ready_to_scheduled
  );
}

static void _Scheduler_priority_SMP_Insert_ready_lifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Scheduler_priority_Ready_queue_enqueue( thread );
}

static void _Scheduler_priority_SMP_Insert_ready_fifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Scheduler_priority_Ready_queue_enqueue_first( thread );
}

static void _Scheduler_priority_SMP_Do_extract(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  bool is_scheduled = thread->is_scheduled;

  ( void ) self;

  thread->is_in_the_air = is_scheduled;
  thread->is_scheduled = false;

  if ( is_scheduled ) {
    _Chain_Extract_unprotected( &thread->Object.Node );
  } else {
    _Scheduler_priority_Ready_queue_extract( thread );
  }
}

void _Scheduler_priority_SMP_Block( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Block(
    self,
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

void _Scheduler_priority_SMP_Enqueue_lifo( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_priority_SMP_Enqueue_ordered(
    self,
    thread,
    _Scheduler_simple_Insert_priority_lifo_order,
    _Scheduler_priority_SMP_Insert_ready_lifo,
    _Scheduler_SMP_Insert_scheduled_lifo
  );
}

void _Scheduler_priority_SMP_Enqueue_fifo( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_priority_SMP_Enqueue_ordered(
    self,
    thread,
    _Scheduler_simple_Insert_priority_fifo_order,
    _Scheduler_priority_SMP_Insert_ready_fifo,
    _Scheduler_SMP_Insert_scheduled_fifo
  );
}

void _Scheduler_priority_SMP_Extract( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Extract(
    self,
    thread,
    _Scheduler_priority_SMP_Do_extract
  );
}

void _Scheduler_priority_SMP_Yield( Thread_Control *thread )
{
  ISR_Level level;

  _ISR_Disable( level );

  _Scheduler_priority_SMP_Extract( thread );
  _Scheduler_priority_SMP_Enqueue_fifo( thread );

  _ISR_Enable( level );
}

void _Scheduler_priority_SMP_Schedule( Thread_Control *thread )
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  _Scheduler_SMP_Schedule(
    self,
    thread,
    _Scheduler_priority_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled
  );
}
