/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_set_priority().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
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

static rtems_status_code _RTEMS_tasks_Set_priority(
  Thread_Control          *the_thread,
  const Scheduler_Control *scheduler,
  Priority_Control         new_priority,
  Thread_queue_Context    *queue_context
)
{
  Priority_Control core_new_priority;
  bool             valid;
  Per_CPU_Control *cpu_self;

  core_new_priority = _RTEMS_Priority_To_core(
    scheduler,
    new_priority,
    &valid
  );

  if ( !valid ) {
    _Thread_Wait_release( the_thread, queue_context );
    return RTEMS_INVALID_PRIORITY;
  }

  _Thread_Priority_change(
    the_thread,
    &the_thread->Real_priority,
    core_new_priority,
    PRIORITY_GROUP_LAST,
    queue_context
  );
  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_Wait_release( the_thread, queue_context );
  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_task_set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority_p
)
{
  Thread_Control          *the_thread;
  Thread_queue_Context     queue_context;
  const Scheduler_Control *scheduler;
  Priority_Control         old_priority;
  rtems_status_code        status;

  if ( old_priority_p == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  the_thread = _Thread_Get( id, &queue_context.Lock_context.Lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _RTEMS_tasks_MP_Set_priority( id, new_priority, old_priority_p );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Thread_Wait_acquire_critical( the_thread, &queue_context );

  scheduler = _Thread_Scheduler_get_home( the_thread );
  old_priority = _Thread_Get_priority( the_thread );

  if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
    status = _RTEMS_tasks_Set_priority(
      the_thread,
      scheduler,
      new_priority,
      &queue_context
    );
  } else {
    _Thread_Wait_release( the_thread, &queue_context );
    status = RTEMS_SUCCESSFUL;
  }

  *old_priority_p = _RTEMS_Priority_From_core( scheduler, old_priority );
  return status;
}
