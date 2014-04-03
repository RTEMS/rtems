/**
 * @file
 *
 * @brief Inlined Routines Associated with the Manipulation of the
 * Priority-Based Scheduling Structures
 *
 * This inline file contains all of the inlined routines associated with
 * the manipulation of the priority-based scheduling structures.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITYIMPL_H
#define _RTEMS_SCORE_SCHEDULERPRIORITYIMPL_H

#include <rtems/score/schedulerpriority.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/prioritybitmapimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreScheduler
 */
/**@{**/

RTEMS_INLINE_ROUTINE Scheduler_priority_Control *
  _Scheduler_priority_Self_from_base( Scheduler_Control *scheduler_base )
{
  return (Scheduler_priority_Control *) scheduler_base->information;
}

/**
 * @brief Ready queue initialization.
 *
 * This routine initializes @a ready_queues for priority-based scheduling.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_initialize(
  Chain_Control *ready_queues
)
{
  size_t index;

  /* initialize ready queue structures */
  for( index=0; index <= PRIORITY_MAXIMUM; index++)
    _Chain_Initialize_empty( &ready_queues[index] );
}

RTEMS_INLINE_ROUTINE Scheduler_priority_Per_thread *
_Scheduler_priority_Get_scheduler_info( Thread_Control *thread )
{
  return ( Scheduler_priority_Per_thread * ) thread->scheduler_info;
}

/**
 * @brief Put a thread to the ready queue.
 *
 * This routine puts @a the_thread on to the priority-based ready queue.
 *
 * @param[in] the_thread The thread to enqueue.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue(
  Thread_Control           *the_thread,
  Priority_bit_map_Control *bit_map
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Scheduler_priority_Get_scheduler_info( the_thread );
  Chain_Control *ready_chain = sched_info_of_thread->ready_chain;

  _Chain_Append_unprotected( ready_chain, &the_thread->Object.Node );
  _Priority_bit_map_Add( bit_map, &sched_info_of_thread->Priority_map );
}

/**
 * @brief Put a thread to the head of the ready queue.
 *
 * This routine puts @a the_thread to the head of the ready queue.
 * For priority-based ready queues, the thread will be the first thread
 * at its priority level.
 *
 * @param[in] the_thread The thread to enqueue.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue_first(
  Thread_Control           *the_thread,
  Priority_bit_map_Control *bit_map
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Scheduler_priority_Get_scheduler_info( the_thread );
  Chain_Control *ready_chain = sched_info_of_thread->ready_chain;

  _Chain_Prepend_unprotected( ready_chain, &the_thread->Object.Node );
  _Priority_bit_map_Add( bit_map, &sched_info_of_thread->Priority_map );
}

/**
 * @brief Remove a specific thread from the ready queue.
 *
 * This routine removes a specific thread from the specified
 * priority-based ready queue.
 *
 * @param[in] the_thread The thread to extract.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_extract(
  Thread_Control           *the_thread,
  Priority_bit_map_Control *bit_map
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Scheduler_priority_Get_scheduler_info( the_thread );
  Chain_Control *ready_chain = sched_info_of_thread->ready_chain;

  if ( _Chain_Has_only_one_node( ready_chain ) ) {
    _Chain_Initialize_empty( ready_chain );
    _Priority_bit_map_Remove( bit_map, &sched_info_of_thread->Priority_map );
  } else {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
  }
}

RTEMS_INLINE_ROUTINE void _Scheduler_priority_Extract_body(
  Scheduler_Control *scheduler_base,
  Thread_Control    *the_thread
)
{
  Scheduler_priority_Control *scheduler =
    _Scheduler_priority_Self_from_base( scheduler_base );

  _Scheduler_priority_Ready_queue_extract( the_thread, &scheduler->Bit_map );
}

/**
 * @brief Return a pointer to the first thread.
 *
 * This routines returns a pointer to the first thread on @a ready_queues.
 *
 * @param[in] bit_map The priority bit map of the scheduler instance.
 * @param[in] ready_queues The ready queues of the scheduler instance.
 *
 * @return This method returns the first thread or NULL
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_priority_Ready_queue_first(
  Priority_bit_map_Control *bit_map,
  Chain_Control            *ready_queues
)
{
  Priority_Control index = _Priority_bit_map_Get_highest( bit_map );

  return (Thread_Control *) _Chain_First( &ready_queues[ index ] );
}

/**
 * @brief Requeue a thread on the ready queue.
 *
 * This routine is invoked when a thread changes priority and should be
 * moved to a different position on the ready queue.
 *
 * @param[in] the_thread is a pointer to the thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_requeue(
  Thread_Control            *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Scheduler_priority_Get_scheduler_info( the_thread );
  Chain_Control *ready_chain = sched_info_of_thread->ready_chain;

  if ( !_Chain_Has_only_one_node( ready_chain ) ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );
    _Chain_Append_unprotected( ready_chain, &the_thread->Object.Node );
  }
}

/**
 * @brief Scheduling decision logic.
 *
 * This kernel routine implements scheduling decision logic
 * for priority-based scheduling.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Schedule_body(
  Scheduler_Control *scheduler_base,
  Thread_Control    *the_thread,
  bool               force_dispatch
)
{
  Scheduler_priority_Control *scheduler =
    _Scheduler_priority_Self_from_base( scheduler_base );
  Thread_Control *heir = _Scheduler_priority_Ready_queue_first(
    &scheduler->Bit_map,
    &scheduler->Ready[ 0 ]
  );

  ( void ) the_thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

RTEMS_INLINE_ROUTINE void _Scheduler_priority_Update_body(
  Thread_Control           *thread,
  Priority_bit_map_Control *bit_map,
  Chain_Control            *ready_queues
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Scheduler_priority_Get_scheduler_info( thread );

  sched_info_of_thread->ready_chain =
    &ready_queues[ thread->current_priority ];

  _Priority_bit_map_Initialize_information(
    bit_map,
    &sched_info_of_thread->Priority_map,
    thread->current_priority
  );
}

/**
 * @brief Priority comparison.
 *
 * This routine implements priority comparison for priority-based
 * scheduling.
 *
 * @return >0 for higher priority, 0 for equal and <0 for lower priority.
 */
RTEMS_INLINE_ROUTINE int _Scheduler_priority_Priority_compare_body(
  Priority_Control      p1,
  Priority_Control      p2
)
{
  /* High priority in priority scheduler is represented by low numbers. */
  return ( p2 - p1 );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
