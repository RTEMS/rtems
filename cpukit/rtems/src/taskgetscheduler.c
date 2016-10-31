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

#include <rtems/rtems/tasks.h>
#include <rtems/score/schedulerimpl.h>

rtems_status_code rtems_task_get_scheduler(
  rtems_id  task_id,
  rtems_id *scheduler_id
)
{
  Thread_Control          *the_thread;
  ISR_lock_Context         lock_context;
  const Scheduler_Control *scheduler;

  if ( scheduler_id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_thread = _Thread_Get( task_id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( task_id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  _Thread_State_acquire_critical( the_thread, &lock_context );

  scheduler = _Thread_Scheduler_get_home( the_thread );
  *scheduler_id = _Scheduler_Build_id( _Scheduler_Get_index( scheduler ) );

  _Thread_State_release( the_thread, &lock_context );
  return RTEMS_SUCCESSFUL;
}
