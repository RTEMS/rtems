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
  rtems_task_priority *old_priority_p
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

    old_priority = _MRSP_Get_ceiling_priority( mrsp, scheduler_index );

    if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
      _MRSP_Set_ceiling_priority( mrsp, scheduler_index, new_priority );
    }

    sc = RTEMS_SUCCESSFUL;
  } else
#endif
  if ( _Attributes_Is_priority_ceiling( attribute_set ) ) {
    CORE_mutex_Control *mutex = &the_semaphore->Core_control.mutex;

    old_priority = mutex->Attributes.priority_ceiling;

    if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
      mutex->Attributes.priority_ceiling = new_priority;
    }

    sc = RTEMS_SUCCESSFUL;
  } else {
    old_priority = 0;

    sc = RTEMS_NOT_DEFINED;
  }

  *old_priority_p = _RTEMS_tasks_Priority_from_Core( old_priority );

  _Objects_Put( &the_semaphore->Object );

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
  Objects_Locations  location;

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

  the_semaphore = _Semaphore_Get( semaphore_id, &location );
  switch ( location ) {
    case OBJECTS_LOCAL:
      return _Semaphore_Set_priority(
        the_semaphore,
        scheduler_id,
        new_priority,
        old_priority
      );
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
