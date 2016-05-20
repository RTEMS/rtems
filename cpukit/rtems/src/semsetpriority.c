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
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/schedulerimpl.h>

static rtems_status_code _Semaphore_Set_priority(
  Semaphore_Control   *the_semaphore,
  rtems_id             scheduler_id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority_p,
  ISR_lock_Context    *lock_context
)
{
  rtems_status_code   sc;
  rtems_attribute     attribute_set = the_semaphore->attribute_set;
  rtems_task_priority old_priority;

  new_priority = _RTEMS_tasks_Priority_to_Core( new_priority );

#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    MRSP_Control *mrsp = &the_semaphore->Core_control.mrsp;
    uint32_t scheduler_index = _Scheduler_Get_index_by_id( scheduler_id );

    _MRSP_Acquire_critical( mrsp, lock_context );

    old_priority = _MRSP_Get_ceiling_priority( mrsp, scheduler_index );

    if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
      _MRSP_Set_ceiling_priority( mrsp, scheduler_index, new_priority );
    }

    _MRSP_Release( mrsp, lock_context );

    sc = RTEMS_SUCCESSFUL;
  } else
#endif
  if ( _Attributes_Is_priority_ceiling( attribute_set ) ) {
    CORE_mutex_Control *mutex = &the_semaphore->Core_control.mutex;

    _CORE_mutex_Acquire_critical( mutex, lock_context );

    old_priority = mutex->Attributes.priority_ceiling;

    if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
      mutex->Attributes.priority_ceiling = new_priority;
    }

    _CORE_mutex_Release( mutex, lock_context );

    sc = RTEMS_SUCCESSFUL;
  } else {
    _ISR_lock_ISR_enable( lock_context );

    old_priority = 0;

    sc = RTEMS_NOT_DEFINED;
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
  Semaphore_Control *the_semaphore;
  ISR_lock_Context   lock_context;

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

  the_semaphore = _Semaphore_Get( semaphore_id, &lock_context );

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
    &lock_context
  );
}
