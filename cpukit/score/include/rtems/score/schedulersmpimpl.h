/**
 * @file
 *
 * @brief SMP Scheduler Implementation
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

#ifndef _RTEMS_SCORE_SCHEDULERSMPIMPL_H
#define _RTEMS_SCORE_SCHEDULERSMPIMPL_H

#include <rtems/score/schedulersmp.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/schedulersimpleimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreSchedulerSMP
 *
 * @{
 */

typedef Thread_Control *( *Scheduler_SMP_Get_highest_ready )(
  Scheduler_SMP_Context *self
);

typedef void ( *Scheduler_SMP_Extract )(
  Scheduler_SMP_Context *self,
  Thread_Control *thread
);

typedef void ( *Scheduler_SMP_Insert )(
  Scheduler_SMP_Context *self,
  Thread_Control *thread_to_insert
);

typedef void ( *Scheduler_SMP_Move )(
  Scheduler_SMP_Context *self,
  Thread_Control *thread_to_move
);

static inline void _Scheduler_SMP_Initialize(
  Scheduler_SMP_Context *self
)
{
  _Chain_Initialize_empty( &self->Scheduled );
}

static inline Scheduler_SMP_Node *_Scheduler_SMP_Node_get(
  Thread_Control *thread
)
{
  return (Scheduler_SMP_Node *) _Scheduler_Node_get( thread );
}

static inline void _Scheduler_SMP_Node_initialize(
  Scheduler_SMP_Node *node
)
{
  node->state = SCHEDULER_SMP_NODE_BLOCKED;
}

extern const bool _Scheduler_SMP_Node_valid_state_changes[ 4 ][ 4 ];

static inline void _Scheduler_SMP_Node_change_state(
  Scheduler_SMP_Node *node,
  Scheduler_SMP_Node_state new_state
)
{
  _Assert(
    _Scheduler_SMP_Node_valid_state_changes[ node->state ][ new_state ]
  );

  node->state = new_state;
}

static inline bool _Scheduler_SMP_Is_processor_owned_by_us(
  const Scheduler_SMP_Context *self,
  const Per_CPU_Control *cpu
)
{
  return cpu->scheduler_context == &self->Base;
}

static inline void _Scheduler_SMP_Update_heir(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_for_heir,
  Thread_Control *heir
)
{
  cpu_for_heir->heir = heir;

  /*
   * It is critical that we first update the heir and then the dispatch
   * necessary so that _Thread_Get_heir_and_make_it_executing() cannot miss an
   * update.
   */
  _Atomic_Fence( ATOMIC_ORDER_SEQ_CST );

  /*
   * Only update the dispatch necessary indicator if not already set to
   * avoid superfluous inter-processor interrupts.
   */
  if ( !cpu_for_heir->dispatch_necessary ) {
    cpu_for_heir->dispatch_necessary = true;

    if ( cpu_for_heir != cpu_self ) {
      _Per_CPU_Send_interrupt( cpu_for_heir );
    }
  }
}

static inline void _Scheduler_SMP_Allocate_processor(
  Scheduler_SMP_Context *self,
  Thread_Control *scheduled,
  Thread_Control *victim
)
{
  Scheduler_SMP_Node *scheduled_node = _Scheduler_SMP_Node_get( scheduled );
  Per_CPU_Control *cpu_of_scheduled = _Thread_Get_CPU( scheduled );
  Per_CPU_Control *cpu_of_victim = _Thread_Get_CPU( victim );
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  Thread_Control *heir;

  _Scheduler_SMP_Node_change_state(
    scheduled_node,
    SCHEDULER_SMP_NODE_SCHEDULED
  );

  _Assert( _ISR_Get_level() != 0 );

  if ( _Thread_Is_executing_on_a_processor( scheduled ) ) {
    if ( _Scheduler_SMP_Is_processor_owned_by_us( self, cpu_of_scheduled ) ) {
      heir = cpu_of_scheduled->heir;
      _Scheduler_SMP_Update_heir( cpu_self, cpu_of_scheduled, scheduled );
    } else {
      /* We have to force a migration to our processor set */
      _Assert( scheduled->debug_real_cpu->heir != scheduled );
      heir = scheduled;
    }
  } else {
    heir = scheduled;
  }

  if ( heir != victim ) {
    _Thread_Set_CPU( heir, cpu_of_victim );
    _Scheduler_SMP_Update_heir( cpu_self, cpu_of_victim, heir );
  }
}

static inline Thread_Control *_Scheduler_SMP_Get_lowest_scheduled(
  Scheduler_SMP_Context *self
)
{
  Thread_Control *lowest_ready = NULL;
  Chain_Control *scheduled = &self->Scheduled;

  if ( !_Chain_Is_empty( scheduled ) ) {
    lowest_ready = (Thread_Control *) _Chain_Last( scheduled );
  }

  return lowest_ready;
}

static inline void _Scheduler_SMP_Enqueue_ordered(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Chain_Node_order order,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Insert insert_ready,
  Scheduler_SMP_Insert insert_scheduled,
  Scheduler_SMP_Move move_from_ready_to_scheduled,
  Scheduler_SMP_Move move_from_scheduled_to_ready
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  if ( node->state == SCHEDULER_SMP_NODE_IN_THE_AIR ) {
    Thread_Control *highest_ready = ( *get_highest_ready )( self );

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
      _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
      _Scheduler_SMP_Allocate_processor( self, highest_ready, thread );
      ( *insert_ready )( self, thread );
      ( *move_from_ready_to_scheduled )( self, highest_ready );
    } else {
      _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_SCHEDULED );
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
      Scheduler_SMP_Node *lowest_scheduled_node =
        _Scheduler_SMP_Node_get( lowest_scheduled );

      _Scheduler_SMP_Node_change_state(
        lowest_scheduled_node,
        SCHEDULER_SMP_NODE_READY
      );
      _Scheduler_SMP_Allocate_processor( self, thread, lowest_scheduled );
      ( *insert_scheduled )( self, thread );
      ( *move_from_scheduled_to_ready )( self, lowest_scheduled );
    } else {
      _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );
      ( *insert_ready )( self, thread );
    }
  }
}

static inline void _Scheduler_SMP_Schedule_highest_ready(
  Scheduler_SMP_Context *self,
  Thread_Control *victim,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Thread_Control *highest_ready = ( *get_highest_ready )( self );

  _Scheduler_SMP_Allocate_processor( self, highest_ready, victim );

  ( *move_from_ready_to_scheduled )( self, highest_ready );
}

static inline void _Scheduler_SMP_Schedule(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  if ( node->state == SCHEDULER_SMP_NODE_IN_THE_AIR ) {
    _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_BLOCKED );

    _Scheduler_SMP_Schedule_highest_ready(
      self,
      thread,
      get_highest_ready,
      move_from_ready_to_scheduled
    );
  }
}

static inline void _Scheduler_SMP_Block(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Scheduler_SMP_Extract extract,
  Scheduler_SMP_Get_highest_ready get_highest_ready,
  Scheduler_SMP_Move move_from_ready_to_scheduled
)
{
  ( *extract )( self, thread );

  _Scheduler_SMP_Schedule(
    self,
    thread,
    get_highest_ready,
    move_from_ready_to_scheduled
  );
}

static inline void _Scheduler_SMP_Extract(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Scheduler_SMP_Extract extract
)
{
  ( *extract )( self, thread );
}

static inline void _Scheduler_SMP_Insert_scheduled_lifo(
  Scheduler_SMP_Context *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

static inline void _Scheduler_SMP_Insert_scheduled_fifo(
  Scheduler_SMP_Context *self,
  Thread_Control *thread
)
{
  _Chain_Insert_ordered_unprotected(
    &self->Scheduled,
    &thread->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

static inline void _Scheduler_SMP_Start_idle(
  Scheduler_SMP_Context *self,
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
  Scheduler_SMP_Node *node = _Scheduler_SMP_Node_get( thread );

  node->state = SCHEDULER_SMP_NODE_SCHEDULED;

  _Thread_Set_CPU( thread, cpu );
  _Chain_Append_unprotected( &self->Scheduled, &thread->Object.Node );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSMPIMPL_H */
