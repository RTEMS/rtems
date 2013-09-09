/**
 * @file
 *
 * @brief SMP Scheduler Implementation
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

#ifndef _RTEMS_SCORE_SCHEDULERSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERSMPIMPL_H

#include <rtems/score/schedulersmp.h>
#include <rtems/score/schedulersimpleimpl.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreSchedulerSMP
 *
 * @{
 */

typedef Thread_Control *( *Scheduler_SMP_Get_highest_ready )(
  Scheduler_SMP_Control *self
);

typedef void ( *Scheduler_SMP_Extract )(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
);

typedef void ( *Scheduler_SMP_Insert )(
  Scheduler_SMP_Control *self,
  Thread_Control *thread_to_insert
);

typedef void ( *Scheduler_SMP_Move )(
  Scheduler_SMP_Control *self,
  Thread_Control *thread_to_move
);

static inline Scheduler_SMP_Control *_Scheduler_SMP_Instance( void )
{
  return _Scheduler.information;
}

static inline void _Scheduler_SMP_Allocate_processor(
  Thread_Control *scheduled,
  Thread_Control *victim
)
{
  Per_CPU_Control *cpu_of_scheduled = scheduled->cpu;
  Per_CPU_Control *cpu_of_victim = victim->cpu;
  Thread_Control *heir;

  scheduled->is_scheduled = true;
  victim->is_scheduled = false;

  _Per_CPU_Acquire( cpu_of_scheduled );

  if ( scheduled->is_executing ) {
    heir = cpu_of_scheduled->heir;
    cpu_of_scheduled->heir = scheduled;
  } else {
    heir = scheduled;
  }

  _Per_CPU_Release( cpu_of_scheduled );

  if ( heir != victim ) {
    const Per_CPU_Control *cpu_of_executing = _Per_CPU_Get();

    heir->cpu = cpu_of_victim;

    /*
     * FIXME: Here we need atomic store operations with a relaxed memory order.
     * The _CPU_SMP_Send_interrupt() will ensure that the change can be
     * observed consistently.
     */
    cpu_of_victim->heir = heir;
    cpu_of_victim->dispatch_necessary = true;

    if ( cpu_of_victim != cpu_of_executing ) {
      _Per_CPU_Send_interrupt( cpu_of_victim );
    }
  }
}

static inline Thread_Control *_Scheduler_SMP_Get_lowest_scheduled(
  Scheduler_SMP_Control *self
)
{
  Thread_Control *lowest_ready = NULL;
  Chain_Control *scheduled = &self->scheduled;

  if ( !_Chain_Is_empty( scheduled ) ) {
    lowest_ready = (Thread_Control *) _Chain_Last( scheduled );
  }

  return lowest_ready;
}

static inline void _Scheduler_SMP_Enqueue_ordered(
  Scheduler_SMP_Control *self,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled,
  Scheduler_SMP_Move move_from_ready_to_scheduled,
  Scheduler_SMP_Move move_from_scheduled_to_ready
)
{
  if ( thread->is_in_the_air ) {
    Thread_Control *highest_ready = ( *get_highest_ready )( self );

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

      ( *insert_ready )( self, thread );
      ( *move_from_ready_to_scheduled )( self, highest_ready );
    } else {
      thread->is_scheduled = true;

      ( *insert_scheduled )( self, thread );
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

      ( *insert_scheduled )( self, thread );
      ( *move_from_scheduled_to_ready )( self, lowest_scheduled );
    } else {
      ( *insert_ready )( self, thread );
    }
  }
}

static inline void _Scheduler_SMP_Schedule_highest_ready(
  Scheduler_SMP_Control *self,
  Thread_Control *victim,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Thread_Control *highest_ready = ( *get_highest_ready )( self );

  _Scheduler_SMP_Allocate_processor( highest_ready, victim );

  ( *move_from_ready_to_scheduled )( self, highest_ready );
}

static inline void _Scheduler_SMP_Block(
  Scheduler_SMP_Control *self,
  Thread_Control *thread,
  Scheduler_SMP_Extract extract,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  ( *extract )( self, thread );

  if ( thread->is_in_the_air ) {
    thread->is_in_the_air = false;

    _Scheduler_SMP_Schedule_highest_ready(
      self,
      thread,
      get_highest_ready,
      move_from_ready_to_scheduled
    );
  }
}

static inline void _Scheduler_SMP_Extract(
  Scheduler_SMP_Control *self,
  Thread_Control *thread,
  Scheduler_SMP_Extract extract
)
{
  ( *extract )( self, thread );
}

static inline void _Scheduler_SMP_Schedule(
  Scheduler_SMP_Control *self,
  Thread_Control *thread,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  if ( thread->is_in_the_air ) {
    thread->is_in_the_air = false;

    _Scheduler_SMP_Schedule_highest_ready(
      self,
      thread,
      get_highest_ready,
      move_from_ready_to_scheduled
    );
  }
}

static inline void _Scheduler_SMP_Insert_scheduled_lifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static inline void _Scheduler_SMP_Insert_scheduled_fifo(
  Scheduler_SMP_Control *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
