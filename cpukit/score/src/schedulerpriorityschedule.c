/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerPriority
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_priority_Schedule().
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityimpl.h>

void _Scheduler_priority_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  _Scheduler_priority_Schedule_body( scheduler, the_thread, false );
}
