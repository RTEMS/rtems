/** 
 *  @file  rtems/score/schedulerpriority.inl
 *
 *  This inline file contains all of the inlined routines associated with
 *  the manipulation of the priority-based scheduling structures.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
# error "Never use <rtems/score/schedulerpriority.inl> directly; include <rtems/score/schedulerpriority.h> instead."
#endif

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_INL
#define _RTEMS_SCORE_SCHEDULERPRIORITY_INL

#include <rtems/score/wkspace.h>

/**
 *  @addtogroup ScoreScheduler
 * @{
 */

/** @brief  Scheduler priority Ready queue initialize
 *
 *  This routine initializes @a the_ready_queue for priority-based scheduling.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_initialize(void)
{
  size_t         index;
  Chain_Control *ready_queues;

  /* allocate ready queue structures */
  _Scheduler.information = _Workspace_Allocate_or_fatal_error(
    ((size_t) PRIORITY_MAXIMUM + 1) * sizeof(Chain_Control)
  );

  /* initialize ready queue structures */
  ready_queues = (Chain_Control *) _Scheduler.information;
  for( index=0; index <= PRIORITY_MAXIMUM; index++)
    _Chain_Initialize_empty( &ready_queues[index] );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_enqueue
 *
 *  This routine puts @a the_thread on to the priority-based ready queue.
 *  
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue(
  Thread_Control                  *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info;
  Chain_Control                 *ready;

  sched_info = (Scheduler_priority_Per_thread *) the_thread->scheduler_info;
  ready      = sched_info->ready_chain;

  _Priority_bit_map_Add( &sched_info->Priority_map );
  
  _Chain_Append_unprotected( ready, &the_thread->Object.Node );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_Enqueue_first
 *
 *  This routine puts @a the_thread to the head of the ready queue. 
 *  For priority-based ready queues, the thread will be the first thread
 *  at its priority level.
 *  
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue_first(
  Thread_Control                   *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info;

  sched_info = (Scheduler_priority_Per_thread *) the_thread->scheduler_info;

  _Priority_bit_map_Add( &sched_info->Priority_map );

  _Chain_Prepend_unprotected(
    sched_info->ready_chain,
    &the_thread->Object.Node
  );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_extract
 *
 *  This routine removes a specific thread from the specified 
 *  priority-based ready queue.
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_extract(
  Thread_Control        *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info;
  Chain_Control                 *ready;

  sched_info = (Scheduler_priority_Per_thread *) the_thread->scheduler_info;
  ready      = sched_info->ready_chain;

  if ( _Chain_Has_only_one_node( ready ) ) {
    _Chain_Initialize_empty( ready );
    _Priority_bit_map_Remove( &sched_info->Priority_map );
  } else {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
  }
}

/**
 *  @brief _Scheduler_priority_Ready_queue_first
 *
 *  This routines returns a pointer to the first thread on @a the_ready_queue.
 *
 *  @param[in] the_ready_queue - pointer to thread queue
 *
 *  @return This method returns the first thread or NULL
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_priority_Ready_queue_first(
  Chain_Control       *the_ready_queue
)
{
  Priority_Control index = _Priority_bit_map_Get_highest();

  if ( !_Chain_Is_empty( &the_ready_queue[ index ] ) )
    return (Thread_Control *) _Chain_First( &the_ready_queue[ index ] );

  return NULL;
}

/**
 *  @brief _Scheduler_priority_Ready_queue_requeue
 *
 *  This routine is invoked when a thread changes priority and should be
 *  moved to a different position on the ready queue.
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_requeue(
  Thread_Control            *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info;

  sched_info = (Scheduler_priority_Per_thread *) the_thread->scheduler_info;

  if ( !_Chain_Has_only_one_node( sched_info->ready_chain ) ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );

    _Chain_Append_unprotected(
      sched_info->ready_chain, 
      &the_thread->Object.Node
    );
  }
}

/**
 *  @brief _Scheduler_priority_Schedule_body
 *
 *  This kernel routine implements scheduling decision logic
 *  for priority-based scheduling.  
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Schedule_body(void)
{
  _Thread_Heir = _Scheduler_priority_Ready_queue_first(
    (Chain_Control *) _Scheduler.information
  );
}

/**
 *  @brief Scheduler priority Priority compare body
 *
 *  This routine implements priority comparison for priority-based
 *  scheduling.
 *
 *  @return >0 for higher priority, 0 for equal and <0 for lower priority.
 */
RTEMS_INLINE_ROUTINE int _Scheduler_priority_Priority_compare_body(
  Priority_Control      p1,
  Priority_Control      p2
)
{
  /* High priority in priority scheduler is represented by low numbers. */
  return ( p2 - p1 );
}

/**@}*/

#endif
/* end of include file */
