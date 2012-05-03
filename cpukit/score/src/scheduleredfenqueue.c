/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>

void _Scheduler_EDF_Enqueue(
  Thread_Control    *the_thread
)
{
  Scheduler_EDF_Per_thread *sched_info =
    (Scheduler_EDF_Per_thread*) the_thread->scheduler_info;
  RBTree_Node *node = &(sched_info->Node);

  _RBTree_Insert( &_Scheduler_EDF_Ready_queue, node );
  sched_info->queue_state = SCHEDULER_EDF_QUEUE_STATE_YES;
}
