/**
 * @file
 *
 * @brief Default Scheduler Release Job Operation
 *
 * @ingroup ScoreScheduler
 */

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

#include <rtems/score/scheduler.h>

void _Scheduler_default_Release_job(
  Thread_Control *thread,
  uint32_t        deadline
)
{
  ( void ) thread;
  ( void ) deadline;
}
