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

static Scheduler_priority_affinity_SMP_Node *
_Scheduler_priority_affinity_Node_get( Thread_Control *thread )
{
  return ( Scheduler_priority_affinity_SMP_Node * )
    _Scheduler_Node_get( thread );
}

bool _Scheduler_priority_affinity_SMP_Allocate(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_priority_affinity_SMP_Node *node =
    _Scheduler_priority_affinity_Node_get( the_thread );

  _Scheduler_SMP_Node_initialize( &node->Base.Base );

  node->Affinity = *_CPU_set_Default();
  node->Affinity.set = &node->Affinity.preallocated;

  return true;
}

bool _Scheduler_priority_affinity_SMP_Get_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  Scheduler_priority_affinity_SMP_Node *node =
    _Scheduler_priority_affinity_Node_get(thread);

  (void) scheduler;

  if ( node->Affinity.setsize != cpusetsize ) {
    return false;
  }

  CPU_COPY( cpuset, node->Affinity.set );
  return true; 
}

bool _Scheduler_priority_affinity_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  Scheduler_priority_affinity_SMP_Node *node =
    _Scheduler_priority_affinity_Node_get(thread);

  (void) scheduler;
  
  if ( ! _CPU_set_Is_valid( cpuset, cpusetsize ) ) {
    return false;
  }

  CPU_COPY( node->Affinity.set, cpuset );
  
  return true;
}
