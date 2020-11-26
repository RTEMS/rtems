/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_get_priority().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_get_priority(
  rtems_id             task_id,
  rtems_id             scheduler_id,
  rtems_task_priority *priority
)
{
  Thread_Control          *the_thread;
  Thread_queue_Context     queue_context;
  const Scheduler_Control *scheduler;
  const Scheduler_Node    *scheduler_node;
  Priority_Control         core_priority;

  if ( priority == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  scheduler = _Scheduler_Get_by_id( scheduler_id );
  if ( scheduler == NULL ) {
    return RTEMS_INVALID_ID;
  }

  _Thread_queue_Context_initialize( &queue_context );
  the_thread = _Thread_Get( task_id,
    &queue_context.Lock_context.Lock_context
  );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( task_id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  scheduler_node = _Thread_Scheduler_get_node_by_index(
     the_thread,
    _Scheduler_Get_index( scheduler )
  );

  _Thread_Wait_acquire_critical( the_thread, &queue_context );

#if defined(RTEMS_SMP)
  if ( _Priority_Is_empty( &scheduler_node->Wait.Priority ) ) {
    _Thread_Wait_release( the_thread, &queue_context );
    return RTEMS_NOT_DEFINED;
  }
#endif

  core_priority = _Priority_Get_priority( &scheduler_node->Wait.Priority );

  _Thread_Wait_release( the_thread, &queue_context );
  *priority = _RTEMS_Priority_From_core( scheduler, core_priority );
  return RTEMS_SUCCESSFUL;
}
