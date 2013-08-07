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
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 * @addtogroup ScoreScheduler
 */
/**@{**/

/**
 * This routine puts @a the_thread on to the ready queue.
 *
 * @param[in] the_ready_queue is a pointer to the ready queue head
 * @param[in] the_thread is the thread to be blocked
 */
RTEMS_INLINE_ROUTINE void _Scheduler_simple_Ready_queue_requeue(
  Scheduler_Control *the_ready_queue,
  Thread_Control    *the_thread
)
{
  /* extract */
  _Chain_Extract_unprotected( &the_thread->Object.Node );

  /* enqueue */
  _Scheduler_simple_Ready_queue_enqueue( the_thread );
}

RTEMS_INLINE_ROUTINE bool _Scheduler_simple_Insert_priority_lifo_order(
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const Thread_Control *thread_to_insert = (const Thread_Control *) to_insert;
  const Thread_Control *thread_next = (const Thread_Control *) next;

  return thread_to_insert->current_priority <= thread_next->current_priority;
}

RTEMS_INLINE_ROUTINE bool _Scheduler_simple_Insert_priority_fifo_order(
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const Thread_Control *thread_to_insert = (const Thread_Control *) to_insert;
  const Thread_Control *thread_next = (const Thread_Control *) next;

  return thread_to_insert->current_priority < thread_next->current_priority;
}

RTEMS_INLINE_ROUTINE void _Scheduler_simple_Insert_priority_lifo(
  Chain_Control *chain,
  Thread_Control *to_insert
)
{
  _Chain_Insert_ordered_unprotected(
    chain,
    &to_insert->Object.Node,
    _Scheduler_simple_Insert_priority_lifo_order
  );
}

RTEMS_INLINE_ROUTINE void _Scheduler_simple_Insert_priority_fifo(
  Chain_Control *chain,
  Thread_Control *to_insert
)
{
  _Chain_Insert_ordered_unprotected(
    chain,
    &to_insert->Object.Node,
    _Scheduler_simple_Insert_priority_fifo_order
  );
}

RTEMS_INLINE_ROUTINE void _Scheduler_simple_Schedule_body(
  Thread_Control *thread,
  bool force_dispatch
)
{
  Thread_Control *heir = (Thread_Control *) _Chain_First(
    (Chain_Control *) _Scheduler.information
  );

  ( void ) thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
