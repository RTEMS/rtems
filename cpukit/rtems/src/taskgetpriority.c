/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_get_priority().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
