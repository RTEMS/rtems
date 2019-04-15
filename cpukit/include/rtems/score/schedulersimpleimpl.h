/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSimple
 *
 * @brief Inlined Routines Associated with the Manipulation of the
 * Priority-Based Scheduling Structures
 *
 * This inline file contains all of the inlined routines associated with
 * the manipulation of the priority-based scheduling structures.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLEIMPL_H
#define _RTEMS_SCORE_SCHEDULERSIMPLEIMPL_H

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/schedulerimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreSchedulerSimple
 *
 * @{
 */

/**
 * @brief Gets context of the scheduler.
 *
 * @param scheduler The scheduler instance to get the context of.
 *
 * @return The context of @a scheduler.
 */
RTEMS_INLINE_ROUTINE Scheduler_simple_Context *
  _Scheduler_simple_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_simple_Context *) _Scheduler_Get_context( scheduler );
}

/**
 * @brief Checks if the priority is less or equal than the priority of the node.
 *
 * @param to_insert The priority to check whether it is less or equal than @a next.
 * @param next The Chain node to compare the priority of.
 *
 * @retval true @a to_insert is smaller or equal than the priority of @a next.
 * @retval false @a to_insert is greater than the priority of @a next.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_simple_Priority_less_equal(
  const void       *to_insert,
  const Chain_Node *next
)
{
  const unsigned int   *priority_to_insert;
  const Thread_Control *thread_next;

  priority_to_insert = (const unsigned int *) to_insert;
  thread_next = (const Thread_Control *) next;

  return *priority_to_insert <= _Thread_Get_priority( thread_next );
}

/**
 * @brief Inserts the thread control with the given priority into the chain.
 *
 * @param[in, out] chain The chain to insert @a to_insert in.
 * @param[in, out] to_insert The node to insert into @a chain.
 * @param insert_priority The priority to insert @a to_insert with.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_simple_Insert(
  Chain_Control  *chain,
  Thread_Control *to_insert,
  unsigned int    insert_priority
)
{
  _Chain_Insert_ordered_unprotected(
    chain,
    &to_insert->Object.Node,
    &insert_priority,
    _Scheduler_simple_Priority_less_equal
  );
}

/**
 * @brief Extracts the threads node.
 *
 * @param scheduler This parameter is unused.
 * @param[in, out] the_thread The thread of which to extract the node out of its chain.
 * @param node This parameter is unused.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_simple_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  (void) scheduler;
  (void) node;

  _Chain_Extract_unprotected( &the_thread->Object.Node );
}

/**
 * @brief Scheduling decision logic.
 *
 * This kernel routine implements scheduling decision logic for the simple scheduler.
 *
 * @param[in, out] scheduler The scheduler instance.
 * @param the_thread This parameter is unused.
 * @param force_dispatch Indicates whether the dispatch happens also if
 *      the currently executing thread is set as not preemptible.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_simple_Schedule_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  bool                     force_dispatch
)
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );
  Thread_Control *heir = (Thread_Control *) _Chain_First( &context->Ready );

  ( void ) the_thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
