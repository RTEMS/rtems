/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This source file contains the implementation of
 *   rtems_semaphore_set_priority().
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

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/schedulerimpl.h>

static rtems_status_code _Semaphore_Set_priority(
  Semaphore_Control       *the_semaphore,
  const Scheduler_Control *scheduler,
  rtems_task_priority      new_priority,
  rtems_task_priority     *old_priority_p,
  Thread_queue_Context    *queue_context
)
{
  bool               valid;
  Priority_Control   core_priority;
  Priority_Control   old_priority;
  Per_CPU_Control   *cpu_self;
  Semaphore_Variant  variant;

  core_priority = _RTEMS_Priority_To_core( scheduler, new_priority, &valid );
  if ( new_priority != RTEMS_CURRENT_PRIORITY && !valid ) {
    _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
    return RTEMS_INVALID_PRIORITY;
  }

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_queue_Acquire_critical(
    &the_semaphore->Core_control.Wait_queue,
    queue_context
  );
  variant = _Semaphore_Get_variant( _Semaphore_Get_flags( the_semaphore ) );

  switch ( variant ) {
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
#if defined(RTEMS_SMP)
      if (
        scheduler != _CORE_ceiling_mutex_Get_scheduler(
          &the_semaphore->Core_control.Mutex
        )
      ) {
        _Thread_queue_Release(
          &the_semaphore->Core_control.Wait_queue,
          queue_context
        );

        return RTEMS_NOT_DEFINED;
      }
#endif

      old_priority = _CORE_ceiling_mutex_Get_priority(
        &the_semaphore->Core_control.Mutex
      );

      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        _CORE_ceiling_mutex_Set_priority(
          &the_semaphore->Core_control.Mutex,
          core_priority
        );
      }

      break;
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      old_priority = _MRSP_Get_priority(
        &the_semaphore->Core_control.MRSP,
        scheduler
      );

      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        _MRSP_Set_priority(
          &the_semaphore->Core_control.MRSP,
          scheduler,
          core_priority
        );
      }

      break;
#endif
    default:
      _Assert(
        variant == SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY
          || variant == SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL
          || variant == SEMAPHORE_VARIANT_SIMPLE_BINARY
          || variant == SEMAPHORE_VARIANT_COUNTING
      );
      _Thread_queue_Release(
        &the_semaphore->Core_control.Wait_queue,
        queue_context
      );

      return RTEMS_NOT_DEFINED;
  }

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Release(
    &the_semaphore->Core_control.Wait_queue,
    queue_context
  );
  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );

  *old_priority_p = _RTEMS_Priority_From_core( scheduler, old_priority );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_semaphore_set_priority(
  rtems_id             semaphore_id,
  rtems_id             scheduler_id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
)
{
  const Scheduler_Control *scheduler;
  Semaphore_Control       *the_semaphore;
  Thread_queue_Context     queue_context;

  if ( old_priority == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  scheduler = _Scheduler_Get_by_id( scheduler_id );
  if ( scheduler == NULL ) {
    return RTEMS_INVALID_ID;
  }

  the_semaphore = _Semaphore_Get( semaphore_id, &queue_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Semaphore_MP_Is_remote( semaphore_id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  return _Semaphore_Set_priority(
    the_semaphore,
    scheduler,
    new_priority,
    old_priority,
    &queue_context
  );
}
