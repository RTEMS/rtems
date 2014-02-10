/**
 * @file
 *
 * @brief  RTEMS Task Set Affinity
 * @ingroup ClassicTasks Tasks
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/cpusetimpl.h>

rtems_status_code rtems_task_set_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset 
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  int                    error;

  if ( !cpuset )
    return RTEMS_INVALID_ADDRESS;

  error = _CPU_set_Is_valid( cpuset, cpusetsize );
  if ( error != 0 )
    return RTEMS_INVALID_NUMBER;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      CPU_COPY( the_thread->affinity.set, cpuset );
      _Objects_Put( &the_thread->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
#endif
