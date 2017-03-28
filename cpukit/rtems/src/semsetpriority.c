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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/schedulerimpl.h>

static rtems_status_code _Semaphore_Is_scheduler_valid(
  const CORE_ceiling_mutex_Control *the_mutex,
  const Scheduler_Control          *scheduler
)
{
#if defined(RTEMS_SMP)
  if ( scheduler != _CORE_ceiling_mutex_Get_scheduler( the_mutex ) ) {
    return RTEMS_NOT_DEFINED;
  }
#endif

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code _Semaphore_Set_priority(
  Semaphore_Control       *the_semaphore,
  const Scheduler_Control *scheduler,
  rtems_task_priority      new_priority,
  rtems_task_priority     *old_priority_p,
  Thread_queue_Context    *queue_context
)
{
  rtems_status_code  sc;
  bool               valid;
  Priority_Control   core_priority;
  Priority_Control   old_priority;
  Per_CPU_Control   *cpu_self;

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

  switch ( the_semaphore->variant ) {
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
      sc = _Semaphore_Is_scheduler_valid(
        &the_semaphore->Core_control.Mutex,
        scheduler
      );

      old_priority = _CORE_ceiling_mutex_Get_priority(
        &the_semaphore->Core_control.Mutex
      );

      if ( sc == RTEMS_SUCCESSFUL && new_priority != RTEMS_CURRENT_PRIORITY ) {
        _CORE_ceiling_mutex_Set_priority(
          &the_semaphore->Core_control.Mutex,
          core_priority,
          queue_context
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

      sc = RTEMS_SUCCESSFUL;
      break;
#endif
    default:
      _Assert(
        the_semaphore->variant == SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY
          || the_semaphore->variant == SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL
          || the_semaphore->variant == SEMAPHORE_VARIANT_SIMPLE_BINARY
          || the_semaphore->variant == SEMAPHORE_VARIANT_COUNTING
      );
      old_priority = 0;
      sc = RTEMS_NOT_DEFINED;
      break;
  }

  cpu_self = _Thread_Dispatch_disable_critical(
    &queue_context->Lock_context.Lock_context
  );
  _Thread_queue_Release(
    &the_semaphore->Core_control.Wait_queue,
    queue_context
  );
  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );

  *old_priority_p = _RTEMS_Priority_From_core( scheduler, old_priority );
  return sc;
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
