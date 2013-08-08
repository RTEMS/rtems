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

static void _Scheduler_simple_smp_Insert(
  Chain_Control *chain,
  Thread_Control *thread,
  Chain_Node_order order
)
{
  _Chain_Insert_ordered_unprotected( chain, &thread->Object.Node, order );
}

static void _Scheduler_simple_smp_Schedule_highest_ready(
  Scheduler_SMP_Control *self,
  Thread_Control *victim
)
{
  Thread_Control *highest_ready =
    (Thread_Control *) _Chain_First( &self->ready[ 0 ] );

  _Scheduler_SMP_Allocate_processor( highest_ready, victim );

  _Scheduler_simple_smp_Move_from_ready_to_scheduled( self, highest_ready );
}

static void _Scheduler_simple_smp_Enqueue_ordered(
  Thread_Control *thread,
  Chain_Node_order order
)
{
  Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

  if ( thread->is_in_the_air ) {
    Thread_Control *highest_ready =
      _Scheduler_simple_smp_Get_highest_ready( self );

    thread->is_in_the_air = false;

    /*
     * The thread has been extracted from the scheduled chain.  We have to
     * place it now on the scheduled or ready chain.
     *
     * NOTE: Do not exchange parameters to do the negation of the order check.
     */
    if (
      highest_ready != NULL
        && !( *order )( &thread->Object.Node, &highest_ready->Object.Node )
    ) {
      _Scheduler_SMP_Allocate_processor( highest_ready, thread );

      _Scheduler_simple_smp_Insert( &self->ready[ 0 ], thread, order );

      _Scheduler_simple_smp_Move_from_ready_to_scheduled(
        self,
        highest_ready
      );
    } else {
      thread->is_scheduled = true;

      _Scheduler_simple_smp_Insert( &self->scheduled, thread, order );
    }
  } else {
    Thread_Control *lowest_scheduled = _Scheduler_SMP_Get_lowest_scheduled( self );

    /*
     * The scheduled chain is empty if nested interrupts change the priority of
     * all scheduled threads.  These threads are in the air.
     */
    if (
      lowest_scheduled != NULL
        && ( *order )( &thread->Object.Node, &lowest_scheduled->Object.Node )
    ) {
      _Scheduler_SMP_Allocate_processor( thread, lowest_scheduled );

      _Scheduler_simple_smp_Insert( &self->scheduled, thread, order );

      _Scheduler_simple_smp_Move_from_scheduled_to_ready(
        self,
        lowest_scheduled
      );
    } else {
      _Scheduler_simple_smp_Insert( &self->ready[ 0 ], thread, order );
    }
  }
}

void _Scheduler_simple_smp_Block( Thread_Control *thread )
{
  _Chain_Extract_unprotected( &thread->Object.Node );

  if ( thread->is_scheduled ) {
    Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

    _Scheduler_simple_smp_Schedule_highest_ready( self, thread );
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
  thread->is_in_the_air = true;

  _Chain_Extract_unprotected( &thread->Object.Node );
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
  if ( thread->is_in_the_air ) {
    Scheduler_SMP_Control *self = _Scheduler_SMP_Instance();

    thread->is_in_the_air = false;

    _Scheduler_simple_smp_Schedule_highest_ready( self, thread );
  }
}
