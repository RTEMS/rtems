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
 * @addtogroup ScoreSchedulerDPS
 */
/**@{**/

RTEMS_INLINE_ROUTINE Scheduler_priority_Context *
  _Scheduler_priority_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_Context *) _Scheduler_Get_context( scheduler );
}

RTEMS_INLINE_ROUTINE Scheduler_priority_Node *_Scheduler_priority_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_priority_Node *) _Scheduler_Thread_get_node( the_thread );
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

/**
 * @brief Enqueues a node on the specified ready queue.
 *
 * The node is placed as the last element of its priority group.
 *
 * @param[in] node The node to enqueue.
 * @param[in] ready_queue The ready queue.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue(
  Chain_Node                     *node,
  Scheduler_priority_Ready_queue *ready_queue,
  Priority_bit_map_Control       *bit_map
)
{
  Chain_Control *ready_chain = ready_queue->ready_chain;

  _Chain_Append_unprotected( ready_chain, node );
  _Priority_bit_map_Add( bit_map, &ready_queue->Priority_map );
}

/**
 * @brief Enqueues a node on the specified ready queue as first.
 *
 * The node is placed as the first element of its priority group.
 *
 * @param[in] node The node to enqueue as first.
 * @param[in] ready_queue The ready queue.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue_first(
  Chain_Node                     *node,
  Scheduler_priority_Ready_queue *ready_queue,
  Priority_bit_map_Control       *bit_map
)
{
  Chain_Control *ready_chain = ready_queue->ready_chain;

  _Chain_Prepend_unprotected( ready_chain, node );
  _Priority_bit_map_Add( bit_map, &ready_queue->Priority_map );
}

/**
 * @brief Extracts a node from the specified ready queue.
 *
 * @param[in] node The node to extract.
 * @param[in] ready_queue The ready queue.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_extract(
  Chain_Node                     *node,
  Scheduler_priority_Ready_queue *ready_queue,
  Priority_bit_map_Control       *bit_map
)
{
  Chain_Control *ready_chain = ready_queue->ready_chain;

  if ( _Chain_Has_only_one_node( ready_chain ) ) {
    _Chain_Initialize_empty( ready_chain );
    _Priority_bit_map_Remove( bit_map, &ready_queue->Priority_map );
  } else {
    _Chain_Extract_unprotected( node );
  }
}

RTEMS_INLINE_ROUTINE void _Scheduler_priority_Extract_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );
  Scheduler_priority_Node *node = _Scheduler_priority_Thread_get_node( the_thread );

  _Scheduler_priority_Ready_queue_extract(
    &the_thread->Object.Node,
    &node->Ready_queue,
    &context->Bit_map
  );
}

/**
 * @brief Return a pointer to the first node.
 *
 * This routines returns a pointer to the first node on @a ready_queues.
 *
 * @param[in] bit_map The priority bit map of the scheduler instance.
 * @param[in] ready_queues The ready queues of the scheduler instance.
 *
 * @return This method returns the first node.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Scheduler_priority_Ready_queue_first(
  Priority_bit_map_Control *bit_map,
  Chain_Control            *ready_queues
)
{
  Priority_Control index = _Priority_bit_map_Get_highest( bit_map );
  Chain_Node *first = _Chain_First( &ready_queues[ index ] );

  _Assert( first != _Chain_Tail( &ready_queues[ index ] ) );

  return first;
}

/**
 * @brief Scheduling decision logic.
 *
 * This kernel routine implements scheduling decision logic
 * for priority-based scheduling.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Schedule_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  bool                     force_dispatch
)
{
  Scheduler_priority_Context *context =
    _Scheduler_priority_Get_context( scheduler );
  Thread_Control *heir = (Thread_Control *)
    _Scheduler_priority_Ready_queue_first(
      &context->Bit_map,
      &context->Ready[ 0 ]
    );

  ( void ) the_thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/**
 * @brief Updates the specified ready queue data according to the new priority
 * value.
 *
 * @param[in] ready_queue The ready queue.
 * @param[in] new_priority The new priority.
 * @param[in] bit_map The priority bit map of the scheduler instance.
 * @param[in] ready_queues The ready queues of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_update(
  Scheduler_priority_Ready_queue *ready_queue,
  Priority_Control                new_priority,
  Priority_bit_map_Control       *bit_map,
  Chain_Control                  *ready_queues
)
{
  ready_queue->ready_chain = &ready_queues[ new_priority ];

  _Priority_bit_map_Initialize_information(
    bit_map,
    &ready_queue->Priority_map,
    new_priority
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
