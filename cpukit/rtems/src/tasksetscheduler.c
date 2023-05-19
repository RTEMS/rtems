/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_set_scheduler().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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
