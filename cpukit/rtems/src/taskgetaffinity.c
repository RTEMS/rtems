/**
 * @file
 *
 * @ingroup ClassicTasks Tasks
 *
 * @brief  RTEMS Task Get Affinity
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_task_get_affinity(
  rtems_id   id,
  size_t     cpusetsize,
  cpu_set_t *cpuset
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;
  bool              ok;

  if ( cpuset == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _Thread_State_acquire_critical( the_thread, &lock_context );

  ok = _Scheduler_Get_affinity(
    the_thread,
    cpusetsize,
    cpuset
  );

  _Thread_State_release( the_thread, &lock_context );
  _Thread_Dispatch_enable( cpu_self );
  return ok ? RTEMS_SUCCESSFUL : RTEMS_INVALID_NUMBER;
}
