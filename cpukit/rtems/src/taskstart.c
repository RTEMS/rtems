/**
 * @file
 *
 * @ingroup ClassicTasks Tasks
 *
 * @brief  RTEMS Start Task
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_start(
  rtems_id            id,
  rtems_task_entry    entry_point,
  rtems_task_argument argument
)
{
  Thread_Entry_information entry = {
    .adaptor = _Thread_Entry_adaptor_numeric,
    .Kinds = {
      .Numeric = {
        .entry = entry_point,
        .argument = argument
      }
    }
  };
  Thread_Control   *the_thread;
  ISR_lock_Context  lock_context;
  bool              ok;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( _Thread_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  ok = _Thread_Start( the_thread, &entry, &lock_context );

  return ok ? RTEMS_SUCCESSFUL : RTEMS_INCORRECT_STATE;
}
