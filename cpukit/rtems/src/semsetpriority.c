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

static rtems_status_code _Semaphore_Set_priority(
  Semaphore_Control    *the_semaphore,
  rtems_id              scheduler_id,
  rtems_task_priority   new_priority,
  rtems_task_priority  *old_priority_p,
  Thread_queue_Context *queue_context
)
{
  rtems_status_code    sc;
  rtems_task_priority  old_priority;
#if defined(RTEMS_SMP)
  MRSP_Control        *mrsp;
  uint32_t             scheduler_index;
#endif

  new_priority = _RTEMS_tasks_Priority_to_Core( new_priority );

  switch ( the_semaphore->variant ) {
    case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
      _CORE_mutex_Acquire_critical(
        &the_semaphore->Core_control.Mutex.Recursive.Mutex,
        queue_context
      );

      old_priority = the_semaphore->Core_control.Mutex.priority_ceiling;

      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        the_semaphore->Core_control.Mutex.priority_ceiling = new_priority;
      }

      _CORE_mutex_Release(
        &the_semaphore->Core_control.Mutex.Recursive.Mutex,
        queue_context
      );
      sc = RTEMS_SUCCESSFUL;
      break;
#if defined(RTEMS_SMP)
    case SEMAPHORE_VARIANT_MRSP:
      mrsp = &the_semaphore->Core_control.MRSP;
      scheduler_index = _Scheduler_Get_index_by_id( scheduler_id );

      _MRSP_Acquire_critical( mrsp, queue_context );

      old_priority = _MRSP_Get_ceiling_priority( mrsp, scheduler_index );

      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        _MRSP_Set_ceiling_priority( mrsp, scheduler_index, new_priority );
      }

      _MRSP_Release( mrsp, queue_context );
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
      _ISR_lock_ISR_enable( &queue_context->Lock_context );
      old_priority = 0;
      sc = RTEMS_NOT_DEFINED;
      break;
  }

  *old_priority_p = _RTEMS_tasks_Priority_from_Core( old_priority );

  return sc;
}

rtems_status_code rtems_semaphore_set_priority(
  rtems_id             semaphore_id,
  rtems_id             scheduler_id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
)
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;

  if ( new_priority != RTEMS_CURRENT_PRIORITY &&
       !_RTEMS_tasks_Priority_is_valid( new_priority ) ) {
    return RTEMS_INVALID_PRIORITY;
  }

  if ( old_priority == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( !_Scheduler_Is_id_valid( scheduler_id ) ) {
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
    scheduler_id,
    new_priority,
    old_priority,
    &queue_context
  );
}
