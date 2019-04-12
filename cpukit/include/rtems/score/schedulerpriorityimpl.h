/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerDPS
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
 * @addtogroup RTEMSScoreSchedulerDPS
 *
 * @{
 */

/**
 * @brief Gets the context of the scheduler.
 *
 * @param scheduler The scheduler to get the context of.
 *
 * @return The context of the scheduler.
 */
RTEMS_INLINE_ROUTINE Scheduler_priority_Context *
  _Scheduler_priority_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_priority_Context *) _Scheduler_Get_context( scheduler );
}

/**
 * @brief Gets the scheduler node of the thread.
 *
 * @param the_thread The thread to get the scheduler node of.
 *
 * @return The scheduler node of @a the_thread.
 */
RTEMS_INLINE_ROUTINE Scheduler_priority_Node *_Scheduler_priority_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_priority_Node *) _Thread_Scheduler_get_home_node( the_thread );
}

/**
 * @brief Gets the priority node of the scheduler node.
 *
 * @param node The node to get the priority node of.
 *
 * @return The priority node.
 */
RTEMS_INLINE_ROUTINE Scheduler_priority_Node *_Scheduler_priority_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_priority_Node *) node;
}

/**
 * @brief Ready queue initialization.
 *
 * This routine initializes @a ready_queues for priority-based scheduling.
 *
 * @param[out] ready_queues The ready queue to initialize.
 * @param maximum_priority The maximum priority in the ready queue.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_initialize(
  Chain_Control    *ready_queues,
  Priority_Control  maximum_priority
)
{
  size_t index;

  for ( index = 0 ; index <= (size_t) maximum_priority ; ++index ) {
    _Chain_Initialize_empty( &ready_queues[ index ] );
  }
}

/**
 * @brief Enqueues a node on the specified ready queue.
 *
 * The node is placed as the last element of its priority group.
 *
 * @param node The node to enqueue.
 * @param[in, out] ready_queue The ready queue.
 * @param[out] bit_map The priority bit map of the scheduler instance.
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
 * @param node The node to enqueue as first.
 * @param[in, out] ready_queue The ready queue.
 * @param[out] bit_map The priority bit map of the scheduler instance.
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
 * @param node The node to extract.
 * @param[in, out] ready_queue The ready queue.
 * @param[out] bit_map The priority bit map of the scheduler instance.
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
    _Chain_Initialize_node( node );
    _Priority_bit_map_Remove( bit_map, &ready_queue->Priority_map );
  } else {
    _Chain_Extract_unprotected( node );
  }
}

/**
 * @brief Extracts a node from the context of the scheduler.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread of which the node will be extracted.
 * @param[in, out] The node which preserves the ready queue.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Extract_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_priority_Context *context;
  Scheduler_priority_Node    *the_node;

  context = _Scheduler_priority_Get_context( scheduler );
  the_node = _Scheduler_priority_Node_downcast( node );

  _Scheduler_priority_Ready_queue_extract(
    &the_thread->Object.Node,
    &the_node->Ready_queue,
    &context->Bit_map
  );
}

/**
 * @brief Returns a pointer to the first node.
 *
 * This routines returns a pointer to the first node on @a ready_queues.
 *
 * @param bit_map The priority bit map of the scheduler instance.
 * @param ready_queues The ready queues of the scheduler instance.
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
 *
 * @param[in, out] scheduler The scheduler instance.
 * @param the_thread This parameter is unused.
 * @param force_dispatch Indicates whether the dispatch happens also if
 *      the currently executing thread is set as not preemptible.
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
 * @param[in, out] ready_queue The ready queue.
 * @param new_priority The new priority.
 * @param bit_map The priority bit map of the scheduler instance.
 * @param ready_queues The ready queues of the scheduler instance.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_update(
  Scheduler_priority_Ready_queue *ready_queue,
  unsigned int                    new_priority,
  Priority_bit_map_Control       *bit_map,
  Chain_Control                  *ready_queues
)
{
  ready_queue->current_priority = new_priority;
  ready_queue->ready_chain = &ready_queues[ new_priority ];

  _Priority_bit_map_Initialize_information(
    bit_map,
    &ready_queue->Priority_map,
    new_priority
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
