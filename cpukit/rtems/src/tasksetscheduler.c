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

rtems_status_code rtems_task_set_scheduler(
  rtems_id task_id,
  rtems_id scheduler_id
)
{
  const Scheduler_Control *scheduler;
  Thread_Control          *the_thread;
  ISR_lock_Context         lock_context;
  ISR_lock_Context         state_lock_context;
  Per_CPU_Control         *cpu_self;
  void                    *lock;
  bool                     ok;

  if ( !_Scheduler_Get_by_id( scheduler_id, &scheduler ) ) {
    return RTEMS_INVALID_ID;
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

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context );

  lock = _Thread_Lock_acquire( the_thread, &lock_context );
  _Thread_State_acquire_critical( the_thread, &state_lock_context );

  ok = _Scheduler_Set( scheduler, the_thread );

  _Thread_State_release_critical( the_thread, &state_lock_context );
  _Thread_Lock_release( lock, &lock_context );
  _Thread_Dispatch_enable( cpu_self );
  return ok ? RTEMS_SUCCESSFUL : RTEMS_INCORRECT_STATE;
}
