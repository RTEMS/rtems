/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_set_scheduler().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_task_set_scheduler(
  rtems_id            task_id,
  rtems_id            scheduler_id,
  rtems_task_priority priority
)
{
  const Scheduler_Control *scheduler;
  Thread_Control          *the_thread;
  Thread_queue_Context     queue_context;
  ISR_lock_Context         state_context;
  Per_CPU_Control         *cpu_self;
  bool                     valid;
  Priority_Control         core_priority;
  Status_Control           status;

  scheduler = _Scheduler_Get_by_id( scheduler_id );
  if ( scheduler == NULL ) {
    return RTEMS_INVALID_ID;
  }

  core_priority = _RTEMS_Priority_To_core( scheduler, priority, &valid );
  if ( !valid ) {
    return RTEMS_INVALID_PRIORITY;
  }

  _Thread_queue_Context_initialize( &queue_context );
  the_thread = _Thread_Get( task_id, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( task_id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  cpu_self = _Thread_queue_Dispatch_disable( &queue_context );
  _Thread_Wait_acquire_critical( the_thread, &queue_context );
  _Thread_State_acquire_critical( the_thread, &state_context );

  status = _Scheduler_Set( scheduler, the_thread, core_priority );

  _Thread_State_release_critical( the_thread, &state_context );
  _Thread_Wait_release( the_thread, &queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return _Status_Get( status );
}
