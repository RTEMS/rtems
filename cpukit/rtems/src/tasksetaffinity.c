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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_task_set_affinity(
  rtems_id         id,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
)
{
  Thread_Control        *the_thread;
  Objects_Locations      location;
  int                    ok;

  if ( !cpuset )
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      ok = _Scheduler_Set_affinity(
        the_thread,
        cpusetsize,
        cpuset
      );
      _Objects_Put( &the_thread->Object );
      return ok ? RTEMS_SUCCESSFUL : RTEMS_INVALID_NUMBER;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
#endif
