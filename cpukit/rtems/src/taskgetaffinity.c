/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_get_affinity().
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
#include <rtems/rtems/statusimpl.h>
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
  Status_Control    status;

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

  _Thread_State_acquire_critical( the_thread, &lock_context );

  status = _Scheduler_Get_affinity(
    the_thread,
    cpusetsize,
    cpuset
  );

  _Thread_State_release( the_thread, &lock_context );
  return _Status_Get( status );
}
