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
 * @addtogroup ScoreSchedulerEDF EDF
 *
 * @{
 */

RTEMS_INLINE_ROUTINE Scheduler_EDF_Context *
  _Scheduler_EDF_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_EDF_Context *) _Scheduler_Get_context( scheduler );
}

RTEMS_INLINE_ROUTINE Scheduler_EDF_Node *_Scheduler_EDF_Node_get(
  Thread_Control *the_thread
)
{
  return (Scheduler_EDF_Node *) _Scheduler_Node_get( the_thread );
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
    _RBTree_Container_of(first, Scheduler_EDF_Node, Node);
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
