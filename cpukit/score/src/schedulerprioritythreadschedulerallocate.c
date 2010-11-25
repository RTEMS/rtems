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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

/*
 *  _Scheduler_priority_Thread_scheduler_allocate
 *
 * Allocates @a the_thread->scheduler
 *
 *  Input parameters:
 *    the_scheduler - pointer to scheduler control
 *    the_thread    - pointer to thread control block
 *
 *  Output parameters: 
 *    Returns pointer to allocated space.
 */

void* _Scheduler_priority_Thread_scheduler_allocate (
  Scheduler_Control     *the_scheduler __attribute__((unused)),
  Thread_Control        *the_thread
)
{
  void                  *sched;

  sched = _Workspace_Allocate( sizeof(Scheduler_priority_Per_thread) );

  the_thread->scheduler.priority = (Scheduler_priority_Per_thread*) sched;

  return sched;
}
