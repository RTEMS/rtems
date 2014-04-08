/*
 *  @file
 *
 *  @brief Scheduler CBS Allocate
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

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>
#include <rtems/score/schedulercbs.h>
#include <rtems/score/wkspace.h>

bool _Scheduler_CBS_Allocate(
  const Scheduler_Control *scheduler,
  Thread_Control    *the_thread
)
{
  Scheduler_CBS_Per_thread *schinfo = the_thread->scheduler_info;

  (void) scheduler;

  schinfo->edf_per_thread.thread = the_thread;
  schinfo->edf_per_thread.queue_state = SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN;
  schinfo->cbs_server = NULL;

  return true;
}
