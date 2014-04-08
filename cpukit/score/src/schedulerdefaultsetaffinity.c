/**
 * @file
 *
 * @brief Scheduler Default Set Affinity Operation
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

bool _Scheduler_default_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  size_t                   cpusetsize,
  const cpu_set_t         *cpuset
)
{
  return _Scheduler_default_Set_affinity_body(
    scheduler,
    thread,
    cpusetsize,
    cpuset
  );
}
