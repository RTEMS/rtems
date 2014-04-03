/**
 *  @file
 *
 *  @brief Free Scheduler Priority
 *  @ingroup ScoreScheduler
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
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
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/wkspace.h>

void _Scheduler_priority_Free (
  Scheduler_Control *base,
  Thread_Control    *the_thread
)
{
  (void) base;

  _Workspace_Free( the_thread->scheduler_info );
}
