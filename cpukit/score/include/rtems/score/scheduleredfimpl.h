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
 *  http://www.rtems.com/license/LICENSE.
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

RTEMS_INLINE_ROUTINE void _Scheduler_EDF_Schedule_body(
  Thread_Control *thread,
  bool force_dispatch
)
{
  RBTree_Node *first = _RBTree_First(&_Scheduler_EDF_Ready_queue, RBT_LEFT);
  Scheduler_EDF_Per_thread *sched_info =
    _RBTree_Container_of(first, Scheduler_EDF_Per_thread, Node);
  Thread_Control *heir = (Thread_Control *) sched_info->thread;

  ( void ) thread;

  _Scheduler_Update_heir( heir, force_dispatch );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
