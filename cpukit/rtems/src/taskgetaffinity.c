/**
 * @file
 *
 * @brief  RTEMS Task Get Affinity
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

rtems_status_code rtems_task_get_affinity(
  rtems_id             id,
  size_t               cpusetsize,
  cpu_set_t           *cpuset
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  rtems_status_code      status = RTEMS_SUCCESSFUL;

  if ( !cpuset )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get( id, &location );

  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( cpusetsize != the_thread->affinity.setsize ) {
        status = RTEMS_INVALID_NUMBER;
      } else {
        CPU_COPY( cpuset, the_thread->affinity.set );
      }
      _Objects_Put( &the_thread->Object );
      return status;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
#endif
