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
#include <rtems/score/schedulercbs.h>
#include <rtems/score/wkspace.h>

void *_Scheduler_CBS_Allocate(
  Thread_Control      *the_thread
)
{
  void *sched;
  Scheduler_CBS_Per_thread *schinfo;

  sched = _Workspace_Allocate(sizeof(Scheduler_CBS_Per_thread));
  if ( sched ) {
    the_thread->scheduler_info = sched;
    schinfo = (Scheduler_CBS_Per_thread *)(the_thread->scheduler_info);
    schinfo->edf_per_thread.thread = the_thread;
    schinfo->edf_per_thread.queue_state = SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN;
    schinfo->cbs_server = NULL;
  }

  return sched;
}
