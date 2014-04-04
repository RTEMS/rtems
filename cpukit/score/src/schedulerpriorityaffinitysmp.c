/**
 * @file
 *
 * @brief Deterministic Priority Affinity SMP Scheduler Implementation
 *
 * @ingroup ScoreSchedulerPriorityAffinitySMP
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

#include <rtems/score/schedulerpriorityaffinitysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/cpusetimpl.h>

RTEMS_INLINE_ROUTINE Scheduler_priority_affinity_SMP_Per_thread *
_Scheduler_priority_affinity_Get_scheduler_info( Thread_Control *thread )
{
  return ( Scheduler_priority_affinity_SMP_Per_thread * ) thread->scheduler_info;
}

void * _Scheduler_priority_affinity_SMP_Allocate(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_affinity_SMP_Per_thread *info =
    _Workspace_Allocate( sizeof( *info ) );

  info->Affinity = *_CPU_set_Default();
  info->Affinity.set = &info->Affinity.preallocated;
 
  the_thread->scheduler_info = info;

  return info;
}

bool _Scheduler_priority_affinity_SMP_Get_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  Scheduler_priority_affinity_SMP_Per_thread *info =
    _Scheduler_priority_affinity_Get_scheduler_info(thread);

  (void) scheduler;

  if ( info->Affinity.setsize != cpusetsize ) {
    return false;
  }

  CPU_COPY( cpuset, info->Affinity.set );
  return true; 
}

bool _Scheduler_priority_affinity_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  Scheduler_priority_affinity_SMP_Per_thread *info =
    _Scheduler_priority_affinity_Get_scheduler_info(thread);

  (void) scheduler;
  
  if ( ! _CPU_set_Is_valid( cpuset, cpusetsize ) ) {
    return false;
  }

  CPU_COPY( info->Affinity.set, cpuset );
  
  return true;
}
