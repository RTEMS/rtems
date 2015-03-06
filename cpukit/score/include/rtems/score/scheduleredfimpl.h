/**
 * @file
 *
 * @ingroup ScoreSchedulerEDF
 *
 * @brief EDF Scheduler Implementation
 */

/*
 *  Copryight (c) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULEREDFIMPL_H
#define _RTEMS_SCORE_SCHEDULEREDFIMPL_H

#include <rtems/score/scheduleredf.h>
#include <rtems/score/schedulerimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreSchedulerEDF
 *
 * @{
 */

RTEMS_INLINE_ROUTINE Scheduler_EDF_Context *
  _Scheduler_EDF_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_EDF_Context *) _Scheduler_Get_context( scheduler );
}

RTEMS_INLINE_ROUTINE Scheduler_EDF_Node *_Scheduler_EDF_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_EDF_Node *) _Scheduler_Thread_get_node( the_thread );
}

RBTree_Compare_result _Scheduler_EDF_Compare(
  const RBTree_Node* n1,
  const RBTree_Node* n2
);

RTEMS_INLINE_ROUTINE void _Scheduler_EDF_Enqueue(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );
  Scheduler_EDF_Node *node = _Scheduler_EDF_Thread_get_node( the_thread );

  _RBTree_Insert(
    &context->Ready,
    &node->Node,
    _Scheduler_EDF_Compare,
    false
  );
  node->queue_state = SCHEDULER_EDF_QUEUE_STATE_YES;
}

RTEMS_INLINE_ROUTINE void _Scheduler_EDF_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );
  Scheduler_EDF_Node *node = _Scheduler_EDF_Thread_get_node( the_thread );

  _RBTree_Extract( &context->Ready, &node->Node );
}

RTEMS_INLINE_ROUTINE void _Scheduler_EDF_Schedule_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  bool                     force_dispatch
)
{
  Scheduler_EDF_Context *context =
    _Scheduler_EDF_Get_context( scheduler );
  RBTree_Node *first = _RBTree_First( &context->Ready, RBT_LEFT );
  Scheduler_EDF_Node *node =
    RTEMS_CONTAINER_OF( first, Scheduler_EDF_Node, Node );
  Thread_Control *heir = node->thread;

  ( void ) the_thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
