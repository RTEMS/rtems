/**
 *  @file
 *
 *  @brief RTEMS Suspend Task
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_suspend(
  rtems_id id
)
{
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;
  States_Control    previous_state;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _RTEMS_tasks_MP_Suspend( id );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  previous_state = _Thread_Set_state( the_thread, STATES_SUSPENDED );

  _Thread_Dispatch_enable( cpu_self );
  return _States_Is_suspended( previous_state ) ?
    RTEMS_ALREADY_SUSPENDED : RTEMS_SUCCESSFUL;
}
