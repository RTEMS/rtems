/*
 *  @file
 *
 *  @brief Initialize Scheduler Priority
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

#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/wkspace.h>

void _Scheduler_priority_Initialize(void)
{
  /* allocate ready queue structures */
  Chain_Control *ready_queues = _Workspace_Allocate_or_fatal_error(
    ((size_t) PRIORITY_MAXIMUM + 1) * sizeof(Chain_Control)
  );

  _Scheduler_priority_Ready_queue_initialize( ready_queues );

  _Scheduler.information = ready_queues;
}
