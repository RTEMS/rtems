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

RTEMS_INLINE_ROUTINE Scheduler_EDF_Control *
  _Scheduler_EDF_Self_from_base( Scheduler_Control *scheduler_base )
{
  return (Scheduler_EDF_Control *) scheduler_base->information;
}

RTEMS_INLINE_ROUTINE void _Scheduler_EDF_Schedule_body(
  Scheduler_Control *scheduler_base,
  Thread_Control    *the_thread,
  bool               force_dispatch
)
{
  Scheduler_EDF_Control *scheduler =
    _Scheduler_EDF_Self_from_base( scheduler_base );
  RBTree_Node *first = _RBTree_First(&scheduler->Ready, RBT_LEFT);
  Scheduler_EDF_Per_thread *sched_info =
    _RBTree_Container_of(first, Scheduler_EDF_Per_thread, Node);
  Thread_Control *heir = (Thread_Control *) sched_info->thread;

  ( void ) the_thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
