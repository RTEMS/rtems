/**
 * @file
 *
 * @brief Scheduler Default Get Affinity Operation
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
#include <rtems/score/cpusetimpl.h>

bool _Scheduler_default_Get_affinity(
  Thread_Control *thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
)
{
  const CPU_set_Control *ctl;

  ctl = _CPU_set_Default();
  if ( cpusetsize != ctl->setsize ) {
    return false;
  }

  CPU_COPY( cpuset, ctl->set );
  
  return true; 
}
