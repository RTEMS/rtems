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
  rtems_id id,
  rtems_id scheduler_id
)
{
  rtems_status_code        sc;
  const Scheduler_Control *scheduler;

  if ( _Scheduler_Get_by_id( scheduler_id, &scheduler ) ) {
    Thread_Control    *the_thread;
    Objects_Locations  location;
    bool               ok;

    the_thread = _Thread_Get( id, &location );

    switch ( location ) {
      case OBJECTS_LOCAL:
        ok = _Scheduler_Set( scheduler, the_thread );
        _Objects_Put( &the_thread->Object );
        sc = ok ? RTEMS_SUCCESSFUL : RTEMS_INCORRECT_STATE;
        break;
#if defined(RTEMS_MULTIPROCESSING)
      case OBJECTS_REMOTE:
        _Thread_Dispatch();
        sc = RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
        break;
#endif
      default:
        sc = RTEMS_INVALID_ID;
        break;
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}
