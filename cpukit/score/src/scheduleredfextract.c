/**
 *  @file
 *
 *  @brief Scheduler EDF Extract
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/scheduleredfimpl.h>

void _Scheduler_EDF_Extract(
  Thread_Control     *the_thread
)
{
  Scheduler_EDF_Control *scheduler =
    _Scheduler_EDF_Instance();
  Scheduler_EDF_Per_thread *sched_info =
    (Scheduler_EDF_Per_thread*) the_thread->scheduler_info;
  RBTree_Node *node = &(sched_info->Node);

  _RBTree_Extract( &scheduler->Ready, node );
  sched_info->queue_state = SCHEDULER_EDF_QUEUE_STATE_NOT_PRESENTLY;
}
