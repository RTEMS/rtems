/**
 *  @file
 *
 *  @brief Allocate Scheduler Priority
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

void *_Scheduler_priority_Allocate (
  Thread_Control        *the_thread
)
{
  Scheduler_priority_Per_thread *sched_info_of_thread =
    _Workspace_Allocate( sizeof( *sched_info_of_thread ) );

  the_thread->scheduler_info = sched_info_of_thread;

  return sched_info_of_thread;
}
