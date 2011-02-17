/*
 *  Scheduler Handler
 *
 *  Copyright (C) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

void *_Scheduler_priority_Thread_scheduler_allocate (
  Thread_Control        *the_thread
)
{
  void  *sched;

  sched = _Workspace_Allocate( sizeof(Scheduler_priority_Per_thread) );

  the_thread->scheduler.priority = (Scheduler_priority_Per_thread *) sched;

  return sched;
}
